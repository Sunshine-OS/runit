#include <unistd.h>
#include "strerr.h"
#include "error.h"
#include "sgetopt.h"
#include "scan.h"
#include "open.h"
#include "tai.h"
#include "buffer.h"
#include "version.h"

#define FATAL "svwaitdone: fatal: "
#define WARN "svwaitdone: warning: "
#define INFO "svwaitdone: "
#define USAGE " [-v] [-t 1..6000] service ..."

const char *progname;
const char * const *dir;
unsigned int rc =0;

void fatal(const char *m) { strerr_die3sys(111, FATAL, m, ": "); }
void warn(const char *s1, const char *s2, struct strerr *e) {
  dir++; rc++;
  strerr_warn3(WARN, s1, s2, e);
}
void usage() { strerr_die4x(1, "usage: ", progname, USAGE, "\n"); }

int main(int argc, const char * const *argv) {
  int opt;
  unsigned long sec =600;
  int verbose =0;
  int dokill =0;
  int wdir;
  int fd;
  char status[20];
  int r;
  unsigned long pid;
  struct tai start;
  struct tai now;
  
  progname =*argv;
  
  while ((opt =getopt(argc, argv, "t:vV")) != opteof) {
    switch(opt) {
    case 't':
      scan_ulong(optarg, &sec);
      if ((sec < 1) || (sec > 6000)) usage();
      break;
    case 'v':
      verbose =1;
      break;
    case 'V':
      strerr_warn1(VERSCODE, 0);
    case '?':
      usage();
    }
  }
  argv +=optind;
  if (! argv || ! *argv) usage();

  if ((wdir =open_read(".")) == -1)
    fatal("unable to open current working directory");

  for (dir =argv; *dir; ++dir) {
    if (dir != argv)
      if (fchdir(wdir) == -1) fatal("unable to switch to starting directory");
    if (chdir(*dir) == -1) continue; /* bummer */
    if ((fd =open_write("supervise/control")) == -1) continue; /* bummer */
    close(fd);
  }
  dir =argv;

  tai_now(&start);
  while (*dir) {
    if (fchdir(wdir) == -1) fatal("unable to switch to starting directory");
    if (chdir(*dir) == -1) {
      warn(*dir, ": unable to change directory: ", &strerr_sys);
      continue;
    }
    if ((fd =open_write("supervise/ok")) == -1) {
      if (errno == error_nodevice) {
        if (verbose) strerr_warn3(INFO, *dir, ": runsv not running.", 0);
        dir++;
      }
      else
        warn(*dir, ": unable to open supervise/ok: ", &strerr_sys);
      continue;
    }
    close(fd);

    if ((fd =open_read("supervise/status")) == -1) {
      warn(*dir, "unable to open supervise/status: ", &strerr_sys);
      continue;
    }
    r =buffer_unixread(fd, status, 20);
    close(fd);
    if ((r < 18) || (r == 19)) { /* supervise compatibility */
      if (r == -1)
        warn(*dir, "unable to read supervise/status: ", &strerr_sys);
      else
        warn(*dir, ": unable to read supervise/status: bad format.", 0);
      continue;
    }
    pid =(unsigned char)status[15];
    pid <<=8; pid +=(unsigned char)status[14];
    pid <<=8; pid +=(unsigned char)status[13];
    pid <<=8; pid +=(unsigned char)status[12];

    if (status[19] == 3) { /* alternative: !pid */
      /* ok, service is down */
      if (verbose) strerr_warn3(INFO, *dir, ": done.", 0);
      dir++;
      continue;
    }

    if (status[19] != '3') { /* catch previous failures */
      if ((fd =open_write("supervise/control")) == -1) {
        warn(*dir, ": unable to open supervise/control: ", &strerr_sys);
        continue;
      }
      close(fd);
    }
  
    tai_now(&now);
    tai_sub(&now, &now, &start);
    if (tai_approx(&now) >= sec) {
      /* timeout */
      if (verbose) strerr_warn2(INFO, "timeout.", 0);
      _exit(111);
    }
    sleep(1);
  }
  if (fchdir(wdir) == -1) 
    strerr_warn2(WARN, "unable to switch to starting directory: ", &strerr_sys);
  close(wdir);
  if (rc > 100) rc =100;
  _exit(rc);
}

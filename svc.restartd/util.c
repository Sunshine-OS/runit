#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include "util.h"

/* Debugging functionality */
void dbg2(const char *funcname, const char *format,  ...)
{
    // Pass on the varargs on to 'vfprintf'. 
    va_list arglist;
    va_start(arglist, format);
    // This may produce the following warning -- ignore it:
    //     warning: format string is not a string literal
	fprintf(stderr, "[dbg] [%s] ", funcname);
    vfprintf(stderr, format, arglist);
    va_end(arglist);
    //fputc('\n', stderr);
}

/* File handling, etc */

int stricmp (const char *p1, const char *p2)
{
  register unsigned char *s1 = (unsigned char *) p1;
  register unsigned char *s2 = (unsigned char *) p2;
  unsigned char c1, c2;
 
  do
  {
      c1 = (unsigned char) toupper((int)*s1++);
      c2 = (unsigned char) toupper((int)*s2++);
      if (c1 == '\0')
      {
            return c1 - c2;
      }
  }
  while (c1 == c2);
 
  return c1 - c2;
}

unsigned long ReadPIDFile(char* PIDFile) /* borrowed from Epoch */
{
	FILE *PIDFileDescriptor = fopen(PIDFile, "r");
	char PIDBuf[200], *TW = NULL, *TW2 = NULL;
	unsigned long InPID = 0, Inc = 0;
	int TChar;

	if (!PIDFileDescriptor)
	{
	return -1; // Zero for failure.
	}

	for (; (TChar = getc(PIDFileDescriptor)) != EOF && Inc < (200 - 1); ++Inc)
	{
		*(unsigned char*)&PIDBuf[Inc] = (unsigned char)TChar;
	}
	PIDBuf[Inc] = '\0';

	fclose(PIDFileDescriptor);

	for (TW = PIDBuf; *TW == '\n' || *TW == '\t' || *TW == ' '; ++TW); 

	for (TW2 = TW; *TW2 != '\0' && *TW2 != '\t' && *TW2 != '\n' && *TW2 != '%' && *TW2 != ' '; ++TW2); // Delete any following the number.
	*TW2 = '\0';

	if (1)//AllNumeric(TW))
	{
		InPID = atoi(TW);
	}
	else
	{
		return -1;
	}
	return InPID;
}

int stat_exists(const char *path)
{
    struct stat st;
    return stat(path,&st) == 0 ? 1 :
           errno == ENOENT ? 0 :
           -1;
}

void check_pidfile(Service* svc)
{
	if (stat_exists(svc->PIDFile))
	{
		int pidfromfile =(int)ReadPIDFile(svc->PIDFile);
		svc->PIDFileRead =1;
		dbg("Changing main PID to: %d\n", pidfromfile);
		svc->MainPID =pidfromfile;
	}
}

/* KEvent/KQueue utilities */

void attach_pid_to_kqueue(int *kq, struct kevent *ke, int pid)
{
	struct kevent e;
	static int i;

	EV_SET(&e, pid, EVFILT_PROC, EV_ADD,
	  NOTE_EXIT | NOTE_FORK | NOTE_TRACK, 0, NULL);
	i = kevent(*kq, &e, 1, NULL, 0, NULL);
	if (i == -1)
		dbg ("proc kevent!\n");
}

void detach_pid_from_kqueue(int *kq, struct kevent *ke, int pid)
{
	struct kevent e;
	static int i;
	EV_SET(&e, ke->ident, EVFILT_PROC, EV_DELETE, 
	  NOTE_EXIT | NOTE_FORK | NOTE_TRACK, 0, NULL);
	i = kevent(*kq, &e, 1, NULL, 0, NULL);
	if (i == -1)
		dbg ("proc kevent!\n");
}

/* Wait wrappers, etc. */

static
int wait_nohang(wstat) 
int *wstat;
{
#ifdef waitpid
    return waitpid(-1,wstat,WNOHANG);
#else
    return wait3(wstat,WNOHANG,(struct rusage *) 0);
#endif
}

void reap()
{
	static int wstat;
	wait_nohang(&wstat);
}

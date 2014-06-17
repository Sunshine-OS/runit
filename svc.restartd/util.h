#ifndef UTIL_H
#define UTIL_H
#include "svc.h"
#include <sys/types.h>
#include <sys/event.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define dbg(format,args...)        \
	dbg2(__func__, format, ## args);
void dbg2(const char *funcname, const char *format,  ...);

int forkexecve(const char* cmd, int *kq, struct kevent *ke, Service* svc, int aux);
void clearsvc(Service* svc);

unsigned long ReadPIDFile(char* PIDFile);
int stat_exists(const char *path);
int check_pidfile(Service* svc);
void attach_pid_to_kqueue(int *kq, struct kevent *ke, int pid);
void detach_pid_from_kqueue(int *kq, struct kevent *ke, int pid);
void reap();
int stricmp (const char *p1, const char *p2);
void purgepids(PIDList_t *pl, int sig);
#endif

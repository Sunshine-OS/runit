#ifndef UTIL_H
#define UTIL_H
#include "svc.h"
#include <sys/types.h>
#include <sys/event.h>

#define dbg(format,args...)        \
                      dbg2(__func__, format, ## args);   
void dbg2(const char *funcname, const char *format,  ...);
unsigned long ReadPIDFile(char* PIDFile);
int stat_exists(const char *path);
void check_pidfile(Service* svc);
void attach_pid_to_kqueue(int *kq, struct kevent *ke, int pid);
void detach_pid_from_kqueue(int *kq, struct kevent *ke, int pid);
void reap();
int stricmp (const char *p1, const char *p2);
#endif

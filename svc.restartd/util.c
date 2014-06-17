#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include "util.h"

/* Misc */

int
PIDsPurged(Service* svc)
{
	if (svc->PL == NULL)
		return 0;
	else
		return 1;
}

int
forkexecve(const char* cmd, int *kq, struct kevent *ke, Service* svc, int aux)
{
	char* data =strdup(cmd);
	char **argv =NULL;
	char *  p    = strtok ((char*)data, " ");
	int n_spaces = 0, i;

	while (p)
	{
		argv = realloc (argv, sizeof (char*) * ++n_spaces);

		if (argv == NULL)
			exit (-1); /* memory allocation failed */

		argv[n_spaces-1] = p;

		p = strtok (NULL, " ");
	}

	/* realloc one extra element for the last NULL */

	argv = realloc (argv, sizeof (char*) * (n_spaces+1));
	argv[n_spaces] = 0;

	/* print the result */

	for (i = 0; i < (n_spaces+1); ++i)
		dbg ("argv[%d] = %s\n", i, argv[i]);

	/* free the memory allocated */
	int pid = fork();

	if ( pid == 0 )
	{
		execvp(argv[0], argv);
	}
	else if (!(pid > 0))
	{
		dbg("fork failed");
		return 0; /* that means it FAILED */
	}
	if(!aux)
		PIDList_addpid(&svc->PL, pid);
	else
		PIDList_addpid(&svc->AuxPL, pid);
	/* watch for forks and exits */
	attach_pid_to_kqueue(kq, ke, pid);

	free (argv);
	return pid;
}

void
clearsvc(Service* svc)
{
	svc->Type =1;
	svc->PIDFile =0;

	svc->ExecStartPre =0;
	svc->ExecStart =0;
	svc->ExecStartPost =0;
	svc->ExecStopPost =0;

	svc->FirstRun =1;
	svc->State =S_INACTIVE;
	svc->Want =S_ONLINE;
	svc->AuxWant =S_NONE;
	svc->MainPIDExited =0;
	svc->AuxMainPIDExited =0;

	svc->PIDFileRead =0;
	svc->OrigPID =0;
	svc->MainPID =0;
	svc->PIDsPurged =1;
	svc->AuxMainPID =0;
	svc->AuxPIDsPurged =1;
	svc->AuxPL =NULL;
	svc->PL =NULL;
}

/* Debugging functionality */
void
dbg2(const char *funcname, const char *format,  ...)
{
	// Pass on the varargs on to 'vfprintf'.
	va_list arglist;
	va_start(arglist, format);
	// This may produce the following warning -- ignore it:
	//     warning: format string is not a string literal
	fprintf(stderr, KRED "[dbg] " KMAG "[%s] " KNRM, funcname);
	vfprintf(stderr, format, arglist);
	va_end(arglist);
	//fputc('\n', stderr);
}

/* File handling, etc */

int
stricmp (const char *p1, const char *p2)
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

unsigned long
ReadPIDFile(char* PIDFile) /* borrowed from Epoch */
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

int
stat_exists(const char *path)
{
	struct stat st;
	return stat(path,&st) == 0 ? 1 :
	       errno == ENOENT ? 0 :
	       -1;
}

int
check_pidfile(Service* svc)
{
	if (stat_exists(svc->PIDFile) && svc->State == S_START)
	{
		int pidfromfile =(int)ReadPIDFile(svc->PIDFile);
		if(pidfromfile > 0)
		{
			svc->PIDFileRead =1;
			dbg("Changing main PID to: %d\n", pidfromfile);
			svc->MainPID =pidfromfile;
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return 1;
}

/* KEvent/KQueue utilities */

void
attach_pid_to_kqueue(int *kq, struct kevent *ke, int pid)
{
	struct kevent e;
	static int i;

	EV_SET(&e, pid, EVFILT_PROC, EV_ADD,
	       NOTE_EXIT | NOTE_FORK | NOTE_TRACK, 0, NULL);
	i = kevent(*kq, &e, 1, NULL, 0, NULL);
	if (i == -1)
		dbg ("proc kevent!\n");
}

void
detach_pid_from_kqueue(int *kq, struct kevent *ke, int pid)
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

static int
wait_nohang(wstat)
int *wstat;
{
#ifdef waitpid
	return waitpid(-1,wstat,WNOHANG);
#else
	return wait3(wstat,WNOHANG,(struct rusage *) 0);
#endif
}

void
reap()
{
	static int wstat;
	wait_nohang(&wstat);
}

void
purgepids(PIDList_t* PL, int sig)
{
	PIDList_t *t;
	t =PL;
	while(t != NULL && t->PID > 0)
	{
		kill(t->PID, sig);
		t=t->Link;
	}
}

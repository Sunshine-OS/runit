#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include "pidlist.h"
#include "ini.h"
#include "util.h"
#include "svc.h"

void
process_proc_kevents(int *kq, struct kevent *ke, Service *svc)
{
	static int i;

	if (ke->fflags & NOTE_FORK)
		dbg("pid %lu called fork()\n", ke->ident);

	if (ke->fflags & NOTE_CHILD)
	{
		dbg("we have a child: %lu\n", ke->ident);
		attach_pid_to_kqueue(kq, ke, ke->ident);
		PIDList_addpid(&svc->PL, ke->ident);
	}

	if (ke->fflags & NOTE_EXIT)
	{
		dbg("pid %lu exited\n", ke->ident);
		detach_pid_from_kqueue(kq, ke, ke->ident);
		PIDList_delpid(&svc->PL, ke->ident);
		reap();

		if (ke->ident == svc->MainPID)
			dbg("Main PID has exited.\n"); svc->MainPIDExited =1;
	}

	if (ke->fflags & NOTE_TRACKERR)
		printf("couldnt attach to child of %lu\n", ke->ident);
}

int
main(int argc, char **argv)
{
	int kq, i;
	struct kevent ke;
	char *procargv[] = {"/usr/sbin/cron", NULL};
	struct timespec tmout = { 3,     /* block for 5 seconds at most */ 
                          0 };   /* nanoseconds */
	Service svc = { 1, 0, 1, S_INACTIVE, 0, 0, 0, 0, NULL };

	if (ini_parse("test.service", parseconfig, &svc) < 0) 
	{
        printf("Can't load 'test.ini'\n");
        return 1;
    }

	kq = kqueue();
	if (kq == -1)
		err(1, "kq!");

	if (svc.PIDFile)
		remove(svc.PIDFile);

	int pid = fork();

	if ( pid == 0 ) {
		execvp( "/usr/sbin/cron", procargv );
	}
	PIDList_addpid(&svc.PL, 0); /* sentinel */
	PIDList_addpid(&svc.PL, pid);
	svc.OrigPID =pid;
	if (svc.Type == T_SIMPLE) svc.MainPID =pid;
	PIDList_print(&svc.PL);

	/* watch for forks and exits */
	attach_pid_to_kqueue(&kq, &ke, pid);

	while (1) 
	{
		memset(&ke, 0x00, sizeof(struct kevent));
		sleep(1);
		/* kevent shall block for 3 seconds - or until something happens */
		i = kevent(kq, NULL, 0, &ke, 1, &tmout);
		if (i == -1)
			err(1, "kevent!");
		process_proc_kevents(&kq, &ke, &svc);
		if (svc.PIDFile && ! svc.PIDFileRead && svc.Type == T_FORKING) 
			check_pidfile(&svc);
		//PIDList_print(&svc.PL);
	}

	return(0);
}

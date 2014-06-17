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
		PIDList_delpid(&svc->AuxPL, ke->ident); /* one of these contains it */
		reap();

		if (ke->ident == svc->MainPID)
		{
			dbg("Main PID has exited.\n");
			svc->MainPIDExited =1;
		}
		else if (ke->ident == svc->AuxMainPID)
		{
			dbg("Main AUX PID has exited.\n");
			svc->AuxMainPIDExited =1;
		}
	}
	if (ke->fflags & NOTE_TRACKERR)
		printf("couldnt attach to child of %lu\n", ke->ident);
}

int
svc_start_pre(int *kq, struct kevent *ke, Service *svc)
{
	if (svc->ExecStartPre)
	{
		int pid=forkexecve(svc->ExecStartPre, kq, ke, svc, 0);
		if(! pid)
			return 1; /* fail */
		else
		{
			dbg("entering state S_START_PRE\n");
			svc->State =S_START_PRE;
			svc->MainPIDExited =0;
			svc->MainPID =pid;
			svc->OrigPID =pid;
		}
	}
	return 0;
}
int
svc_start(int *kq, struct kevent *ke, Service *svc)
{
	int pid=forkexecve(svc->ExecStart, kq, ke, svc, 0);
	if(! pid)
		return 1; /* fail */
	else
	{
		dbg("entering state S_START\n");
		svc->State =S_START;
		svc->MainPIDExited =0;
		svc->OrigPID =pid;
		if(svc->Type != T_FORKING)
			svc->MainPID =pid;
		if(svc->Type == T_SIMPLE)
		{
			dbg("entering state S_ONLINE\n");
			svc->State =S_ONLINE;
		}
	}
	return 0;
}
int
svc_start_post(int *kq, struct kevent *ke, Service *svc)
{
	if (svc->ExecStartPost)
	{
		int pid=forkexecve(svc->ExecStartPost, kq, ke, svc, 1);
		if(! pid)
			return 1; /* fail */
		else
		{
			dbg("entering AUX state S_START_POST\n");
			svc->AuxState =S_START_POST;
			svc->AuxMainPIDExited =0;
			svc->AuxMainPID =pid;
			svc->AuxWant =S_NONE;
		}
	}
	else
	{
		dbg("skipping through AUX state S_START_POST\n");
		svc->AuxState =S_START_POST;
		svc->AuxMainPIDExited =2; /* when this is seen, just skip errorcheck */
		svc->AuxWant =S_NONE;
	}
	return 0;
}
int
svc_start_post_oneshot(int *kq, struct kevent *ke, Service *svc)
{
	if (svc->ExecStartPost)
	{
		int pid=forkexecve(svc->ExecStartPost, kq, ke, svc, 0);
		if(! pid)
			return 1; /* fail */
		else
		{
			dbg("entering state S_START_POST\n");
			svc->State =S_START_POST;
			svc->MainPIDExited =0;
			svc->MainPID =pid;
			svc->OrigPID =pid;
		}
	}
	else
	{
		dbg("skipping threough state S_START_POST\n");
		svc->State =S_START_POST;
		svc->MainPIDExited =2; /* when this is seen, just skip errorcheck */
	}
	return 0;
}
int
svc_stop_post(int *kq, struct kevent *ke, Service *svc)
{
	svc->PIDFileRead =0;
	if (svc->ExecStopPost)
	{
		int pid=forkexecve(svc->ExecStopPost, kq, ke, svc, 0);
		if(! pid)
			return 1; /* fail */
		else
		{
			dbg("entering state S_STOP_POST\n");
			svc->State =S_STOP_POST;
			svc->MainPIDExited =0;
			svc->OrigPID =pid;
			svc->MainPID =pid;
		}
	}
	return 0;
}
int
svc_transition_if_necessary(int *kq, struct kevent *ke, Service *svc)
{
	// if Svc Wants To Be Online
	// and State is inactive, startpre, start
	// then (either advance to startpre or, if startpre has exited, advance
	// to start. If start and type=simple, and PID runs, -> online. If
	// type = forking AND pidfile found, -> online. If MainPIDExited, kill
	if(svc->PIDsPurged)
	{
		if(svc->Want == S_ONLINE && (svc->State == S_INACTIVE ||
		                             svc->State == S_EXITED || svc->State == S_STOP_POST))
		{
			if(svc->State == S_EXITED) /* not doing poststop */
				return svc_stop_post(kq, ke, svc);
			else if((svc->State == S_STOP_POST && svc->MainPIDExited) ||
			        svc->State == S_INACTIVE)
				return svc_start_pre(kq, ke, svc);
		}
		if(svc->Want == S_ONLINE && svc->State == S_START_PRE)
		{
			// check timeout ?
			// check if ExecStartPre exited cleanly too
			if (svc->MainPIDExited)
				return svc_start(kq, ke, svc);
		}
	}
	if ((! svc->PIDFileRead) && svc->PIDFile && svc->State == S_START)
	{
		int res =check_pidfile(svc);
		if(svc->MainPID && !res)
		{
			dbg("entering state S_ONLINE\n");
			//svc->
			svc->State =S_ONLINE;
			svc->AuxWant =S_START_POST;
			return 0;
		}
	}
	if ((svc->State == S_START || svc->State == S_ONLINE) &&
	        svc->PIDsPurged == 1 && svc->Type != T_ONESHOT)
	{
		// IF CLEAN
		dbg("entering state S_EXITED\n");
		svc->State =S_EXITED;
		svc->DoubleTransition =1;
		if (svc->PIDFile)
			remove(svc->PIDFile);
		svc->MainPID =0;
		// IF DODGY
		// Set a 'failed' flag, and when we finish off the S_STOP_POST lark,
		// enter S_FAILED
		return 0;
	}
	if(svc->Type == T_ONESHOT)
	{
		if (svc->State == S_START)
		{
			svc_start_post_oneshot(kq, ke, svc);
		}
		else if (svc->State == S_START_POST)/* oneshot still executing at last pass */
		{
			// Check if exit status was 0 before doing this
			dbg ("entering state S_ONLINE\n");
			svc->State =S_ONLINE;
			return 0;
		}
	}
	if (svc->AuxWant == S_START_POST)
	{
		svc_start_post(kq, ke, svc);
	}
	return 0;
}
int
main(int argc, char **argv)
{
	int kq, i;
	struct kevent ke;
	struct timespec tmout = { 3,     /* block for 5 seconds at most */
		       0
	};   /* nanoseconds */
	Service svc;
	clearsvc(&svc);

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

	svc_transition_if_necessary(&kq, &ke, &svc);
	while (1)
	{
		memset(&ke, 0x00, sizeof(struct kevent));
		sleep(1);

		/* kevent shall block for 3 seconds - or until something happens */
		i = kevent(kq, NULL, 0, &ke, 1, &tmout);
		if (i == -1)
			err(1, "kevent!");
		process_proc_kevents(&kq, &ke, &svc);
		if (svc.PL == NULL)
			svc.PIDsPurged =1;
		else
			svc.PIDsPurged =0;

		if (svc.AuxPL == NULL)
			svc.AuxPIDsPurged =1;
		else
			svc.AuxPIDsPurged =0;

		if (svc.MainPIDExited && (svc.PL != NULL) && svc.Type != T_ONESHOT)
		{
			dbg("purging extra PIDs\n");
			purgepids(svc.PL, SIGTERM);
		}

		if (svc.AuxMainPIDExited && (svc.AuxPL != NULL))
		{
			dbg("purging extra AUX PIDs\n");
			purgepids(svc.AuxPL, SIGTERM);
		}

		svc_transition_if_necessary(&kq, &ke, &svc);
		if (svc.MainPIDExited)
			svc_transition_if_necessary(&kq, &ke, &svc); /* used for S_EXITED */
		// Maybe writing a byte to a self-pipe is better.

		//if (!svc.MainPIDExited) PIDList_print(&svc.PL);
	}

	return(0);
}

#ifndef SVC_H
#define SVC_H

#include "pidlist.h"

#define T_SIMPLE 1
#define T_FORKING 2

struct Service
{
	int Type;
	char* PIDFile;

	int FirstRun;
	int State;
	int MainPIDExited;

	int PIDFileRead;
	int OrigPID;
	int MainPID;
	PIDList_t *PL;
};

typedef struct Service Service;

#define S_INACTIVE 0
#define S_EXITED 1
#define S_START_PRE 2
#define S_START 3
#define S_START_POST 4
#define S_ONLINE 5
#define S_STOP 6
#define S_STOP_SIGTERM 7
#define S_STOP_SIGKILL 8
#define S_STOP_POST 9
#define S_FINAL_SIGTERM 10
#define S_FINAL_SIGKILL 11
#define S_FAILED 12
#define S_RESTART 13

#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pidlist.h"

void PIDList_addpid(PIDList_t **n, int PID)
{

    PIDList_t *temp,*t;
    if(*n==NULL)
    {
        t=*n;
        t=malloc(sizeof(PIDList_t));
        t->PID=PID;
        t->Link=NULL;
        *n=t;
    }
    else
    {
        t=*n;
        temp=malloc(sizeof(PIDList_t));
        while(t->Link!=NULL)
            t=t->Link;
        temp->PID=PID;
        temp->Link=NULL;
        t->Link=temp;
    }
}

void PIDList_delpid(PIDList_t **list, int PID)
{
  PIDList_t *current, *previous;

  /* no previous for 1st node. */
  previous = NULL;
  for (current = *list;
	current != NULL;
	previous = current, current = current->Link) 
	{
		if (current->PID == PID) 
		{ 
		  if (previous == NULL) 
		{
		    /* fix first. */
		    *list = current->Link;
		  } else 
		  {
		    /* skip removing node */
		    previous->Link = current->Link;
		  }

		  free(current);
		  return;
		}
  }
}

void PIDList_print(PIDList_t **n)
{
    PIDList_t *t;
    t=*n;
    if(t==NULL) printf("Empty list\n");

	printf("Begin list.\n");
    while(t!=NULL)
    {
        printf("%d\n",t->PID);
        t=t->Link;
    }
	printf("End list.\n");
	return;
}

#ifndef PIDList_h
#define PIDList_h

struct PIDList
{
    int PID;
    struct PIDList *Link;
};

typedef struct PIDList PIDList_t;

void PIDList_addpid(PIDList_t **n, int PID);
void PIDList_delpid(PIDList_t **n, int PID);
void PIDList_print(PIDList_t **n);

#endif

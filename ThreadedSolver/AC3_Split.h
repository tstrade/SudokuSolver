#ifndef AC3_SPLIT_H
#define AC3_SPLIT_H

#include "CSP.h"
#include "queue.h"

#define FETCH 0
#define EDIT 1
#define SCAN 2
#define PROCESS 3

typedef struct AC3 AC3;
typedef struct Revisors Revisors;
typedef struct HOA HOA;

AC3 *initAC3();
Revisors *initRevisors();
HOA *initHOA();
void *fetchQueue(void *arg);
void *editQueue(void *arg);
void *scanQueue(void *arg);
void *processTuple(void *arg);
void *revise(void *arg);
void *inspect(void *arg);
void cancel(int sig);
void destroyAC3(AC3 *solver);

#endif

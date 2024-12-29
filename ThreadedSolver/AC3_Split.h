#ifndef AC3_SPLIT_H
#define AC3_SPLIT_H

#include <pthread.h>
#include "CSP.h"
#include "queue.h"

void *fetchQueue(void *arg);
void *editQueue(void *arg);
void *scanQueue(void *arg);
void *processTuple(void *arg);
void *revise(void *arg);
void *inspect(void *arg);

#endif

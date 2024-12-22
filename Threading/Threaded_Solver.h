#ifndef THREADED_SOLVER_H
#define THREADED_SOLVER_H

#include "datastructs.h"
#include "CSP.h"
#include "Threads.h"


void *AC3(void *args);
int revise(CSP *csp, int Xi, int Xj, protector *knight);
void get_queue(CSP *csp, Queue *q, int variable);


#endif

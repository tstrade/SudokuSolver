#ifndef SOLVER_H
#define SOLVER_H

#include "datastructs.h"
#include "CSP.h"

typedef struct AC3_args AC3_args;
typedef struct protect protect;

AC3_args *initAC3Args(CSP *csp, Queue *q, int slot, protect *knight);
protect *initProtect();
void *AC3(void *args);
int revise(CSP *csp, int Xi, int Xj, protect *knight);
void get_queue(CSP *csp, Queue *q, int variable);
void destroyAC3Args(AC3_args *args);
void destroyProtect(protect *knight);


#endif

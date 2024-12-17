#ifndef _SOLVER_H
#define _SOLVER_H

#include "datastructs.h"
#include "CSP.h"

int AC3(Queue *q, CSP *csp);
int revise(CSP *csp, int Xi, int Xj);
int *backtracking_search(CSP *csp);
int *backtrack(CSP *csp);
int select_unassigned_variable(CSP *csp);
Queue *get_queue(CSP *csp, Queue *q, int variable);


#endif

#ifndef _SOLVER_H
#define _SOLVER_H

#include "datastructs.h"
#include "CSP.h"

int AC3(Solver *self);
int revise(struct Solver *self, int Xi, int Xj);
int *backtracking_search(Solver *self);
int *backtrack(Solver *self);
int select_unassigned_variable(Solver *self);
Queue *get_queue(Solver *self, int variable);
void destroySolver(Solver *self);


#endif

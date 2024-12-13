#ifndef _SOLVER_H
#define _SOLVER_H

#include "datastructs.h"
#include "CSP.h"

typedef struct Solver Solver;
typedef struct AC3_args AC3_args;
int var_AC3(AC3_args args);
int AC3(Solver *self, CSP *csp, Queue *q, int **removals);
int revise(struct Solver *self, CSP *csp, int Xi, int Xj, int **removals);
void backtracking_search(Solver *self, CSP *csp);
int *backtrack(Solver *self, int *assignment, CSP *csp);
int select_unassigned_variable(Solver *self, int *assignment, CSP *csp);
Queue *get_queue(Solver *self, CSP *csp, int variable);


#endif

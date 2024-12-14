#include "CSP.h"
#include "datastructs.h"
#include "Solver.c"
#include <stdlib.h>

struct Solver {
  Queue *q;
  CSP *csp;
  int **removals;
  int (*AC3)(Solver *self);
  int (*revise)(Solver *self, CSP *csp, int Xi, int Xj, int **removals);
  void (*backtracking_search)(Solver *self, CSP *csp);
  int* (*backtrack)(Solver *self, int *assigment, CSP *csp);
  int (*select_unassigned_variable)(Solver *self, int *assignment, CSP *csp);
  Queue* (*get_queue)(Solver *self, CSP *csp, int variable);
};

Solver *initSolver() {
  Solver *self = malloc(sizeof(Solver));
  if (self == NULL) { return NULL; }
  // Return NULL if not enough memory for Solver structure
  // Same concept is used for allocated memory for the struct members

  // Enqueues/dequeues Variable-Value tuples to be considered
  Queue *q = initQueue(head = NULL);
  if (q == NULL) { return NULL; }

  // Keeps track of current assignments, domains, etc.
  CSP *csp = initCSP();
  if (csp == NULL) { return NULL; }

  // '0' indicates not removed, '1' indicates removed (from possible variable-value pairs)
  int **rmv = calloc(NUM_SLOTS, sizeof(int *));
  if (rmv == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) {
    rmv[i] = calloc(NUM_VALUES, sizeof(int));
    if (rmv[i] == NULL) { return NULL; }
  }

  return self;
}

int AC3(Solver *self) {
  // We know the queue is empty if the first value is set to 0 (it can only be 1 - 9)
  if (self->q->varValTuple[1] == 0) {
    /* for Variable in csp.curr_domains:
	  for Neighbor in self.get_queue(csp, Variable):
	    q.append(Neighbor) */
    int var, val;
    for (var = 0; var < NUM_SLOTS; var++) {
      for (val = 0; val < NUM_VALUES; val++) {

      }
    }
  }
  while (q->next != NULL) {
    /*
    Xi, Xj = queue[0][0], queue.pop(0)[1]
    if self.revise(csp, Xi, Xj, removals):
        if not csp.curr_domains[Xi]: return False
        for Domain in csp.curr_domains:
            if Xi in csp.neighbors[Domain] and (Domain, Xi) not in queue and Domain != Xj:
                queue.append((Domain, Xi))
    */
  }
  return 1;
}

Queue *get_queue(Solver *self, int variable) {
  Queue *q = initQueue(head = NULL);
  // We want to find each variable-variable pair that is in the same row and/or
  // column, which will help us check our possible solution against our constraint
  int checkNeighbors;
  for (checkNeighbors = 0; checkNeighbors < NUM_NEIGHBORS; checkNeighbors++) {
    if ((int neighbor = self->csp->neighbors[var][checkNeighbor]) == 1) {
      int *tuple = [variable, neighbor];
      q->enqueue(q, tuple);
    }
  }
  return q;
}

int main() {
  CSP *csp = initCSP();
}

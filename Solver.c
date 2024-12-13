#include "CSP.h"
#include "datastructs.h"
#include "Solver.c"
#include <stdlib.h>

struct Solver {
  Queue *q;
  CSP *csp;
  int **removals; // List of 81 entries (one for each var) and 9 values for each
  int (*AC3)(Solver *self);
  int (*revise)(Solver *self, CSP *csp, int Xi, int Xj, int **removals);
  void (*backtracking_search)(Solver *self, CSP *csp);
  int* (*backtrack)(Solver *self, int *assigment, CSP *csp);
  int (*select_unassigned_variable)(Solver *self, int *assignment, CSP *csp);
  Queue* (*get_queue)(Solver *self, CSP *csp, int variable);
};

Solver *initSolver() {
  if ((Solver *self = malloc(sizeof(Solver))) == NULL) { return NULL; }
  // Return NULL if not enough memory for Solver structure
  // Same concept is used for allocated memory for the struct members
  if ((Queue *q = initQueue(NUM_SLOTS)) == NULL) { return NULL; }
  if ((CSP *csp = initCSP()) == NULL) { return NULL; }
  if ((int **rmv = malloc(NUM_SLOTS * sizeof(int *))) == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) {
    if ((rmv[i] = malloc(NUM_VALUES * sizeof(int))) == NULL) { return NULL; }
  }

  return self;
}

int AC3(Solver *self) {
  if (!q->currSize) {
    /* for Variable in csp.curr_domains:
	  for Neighbor in self.get_queue(csp, Variable):
	    q.append(Neighbor) */
    int var;
    for (var = 0; var < NUM_SLOTS; var++) {
      q
    }
  }
  while (q != NULL) {
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

int main() {
  CSP *csp = initCSP();
}

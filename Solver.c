#include "CSP.h"
#include "datastructs.h"
#include "Solver.h"
#include <stdlib.h>

struct Solver {
  Queue *q;
  CSP *csp;
  int (*AC3)(Solver *self);
  int* (*backtracking_search)(Solver *self);
};

Solver *initSolver() {
  Solver *self = malloc(sizeof(Solver));
  checkNULL((void *)self);
  // Return NULL if not enough memory for Solver structure
  // Same concept is used for allocated memory for the struct members

  // Enqueues/dequeues Variable-Value tuples to be considered
  self->q = initQueue(head = NULL);
  checkNULL((void *)self->q);

  // Keeps track of current assignments, domains, etc.
  self->csp = initCSP();

  return self;
}

int AC3(Solver *self) {
  // We know the queue is empty if the first value is set to 0 (it can only be 1 - 9)
  if (self->q->varValTuple[1] == 0) {
    int var;
    for (var = 0; var < NUM_SLOTS; var++) {
      get_queue(self, var);
    }
  }
  while (self->q->next != NULL) {
    int Xi = self->q->varValTuple[0], Xj = self->q->varValTuple[1];
    self->q = dequeue(self->q);

    if (revise(self, Xi, Xj) == 1) {
      if (count(self->csp->curr_domains[Xi]) == 0) { return 0; }
      int var, n, isNeighbor, isNotQueued;
      Queue *checkInQueue;
      for (var = 0; var < NUM_SLOTS; var++) {
        isNeighbor = 0, isNotQueued = 1;
        checkInQueue = self->q;

        if (var == Xi) { continue; }

        for (n = 0; n < NUM_NEIGHBORS; n++) {
          if (Xi == self->csp->neighbors[var][n]) {
            isNeighbor = 1;
          }
        }

        while (checkInQueue != NULL) {
          if (checkInQueue->varValTuple[0] == var && checkInQueue->varValTuple[1] == Xi) {
            isNotQueued = 0;
            break;
          }
          checkInQueue = checkInQueue->next;
        }

        if (isNeighbor && isNotQueued) {
          int *item = {var, Xi};
          enqueue(self->q, item);
      }
    }
  }
  return 1;
}

int revise(Solver *self, int Xi, int Xj) {
  int revised = 0; // false

  // Want to see how many values in Xi's domain satisfy the ruls of soduku
  int XiValue, XjValue, valXi, valXj, satisfied;

  for (valXi = 0; valXi < NUM_VALUES; valXi++) {
    // Initialize each time to ensure filled with 0's when calling count()
    int constraintSatisfied[NUM_VALUES] = {0,0,0,0,0,0,0,0,0};

    XiValue = self->csp->curr_domains[Xi][valXi];
    if (XiValue == 0) { continue; }
    // Check each value in Xi's domain against every value in Xj's domain
    for (valXj = 0; valXj < NUM_VALUES; valXj) {
      XjValue = self->csp->curr_domains[Xj][valXj];
      if (XjValue == 0) { continue; }

      // If XiValue is in Xi's domain (not 0) and XjValue is in Xj's domain
      // check if ((Xi != Xj) && (XiValue == XjValue)) and set entry equal
      // to result of satisfaction check (0 if false, 1 if true)
      satisfied = self->csp->constraints(Xi, XiValue, Xj, XjValue);
      constraintSatisfied[valXj] = satisfied;
    }

    if (count(constraintSatisfied) != 0) {
      prune(self->csp, Xi, XiValue);
      revised = 1;
    }
  }
  return revised;
}

int *backtracking_search(Solver *self) {
  int assignment[81], var;
  for (var = 0; var < NUM_SLOTS; var++) { assignment[var] = 0; }
  return backtrack(self, assignment);
}

int *backtrack(Solver *self, int *assignment) {
  if (goal_test(self->csp, assignment) != 0) { return assignment; }

  int val, variable = select_unassigned_variable(self, assignment);
  for (val = 0; val < NUM_VALUES; val++) {
    int conflicts = nconflicts(self->csp, variable, value, assignment);

    if (conflicts == 0) {
      assign(self->csp, variable, value, assignment);
      suppose(self->csp, variable, value);
      int *result = backtrack(self, assignment);

      if (result != NULL) { return result; }
      else { unassigned(variable, assigment); }
    }
  }
  return NULL;
}

int select_unassigned_variable(Solver *self, int *assignment) {
  for (var = 0; var < NUM_SLOTS; var++) {
    if (assignment[var] == 0) { return var; } // Return first unassigned variable
  }
  return -1; // All variables assigned a value
}



void get_queue(Solver *self, int variable) {
  // We want to find each variable-variable pair that is in the same row and/or
  // column, which will help us check our possible solution against our constraint
  int checkNeighbors;
  for (checkNeighbors = 0; checkNeighbors < NUM_NEIGHBORS; checkNeighbors++) {
    if ((int neighbor = self->csp->neighbors[var][checkNeighbor]) == 1) {
      int *tuple = [variable, neighbor];
      enqueue(self->q, tuple);
    }
  }
}

int main() {
  CSP *csp = initCSP();
}

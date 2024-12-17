#include "CSP.c"
#include "datastructs.c"
#include "Solver.h"
#include <stdlib.h>
#include <time.h>

#define NUM_VALUES 9
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#define NUM_NEIGHBORS (2 * (NUM_VALUES - 1))

int AC3(Queue *q, CSP *csp) {
  // We know the queue is empty if the first value is set to 0 (it can only be 1 - 9)
  if (q->varValTuple[1] == 0) {
    int var;
    for (var = 0; var < NUM_SLOTS; var++) {
      get_queue(csp, q, var);
    }
  }
  while (q->next != NULL) {
    int Xi = q->varValTuple[0], Xj = q->varValTuple[1];
    q = dequeue(q);

    if (revise(csp, Xi, Xj) == 1) {
      if (count(csp->curr_domains[Xi]) == 0) { return 0; }
      int var, n, isNeighbor, isNotQueued;
      Queue *checkInQueue;
      for (var = 0; var < NUM_SLOTS; var++) {
        isNeighbor = 0, isNotQueued = 1;
        checkInQueue = q;

        if (var == Xi) { continue; }

        for (n = 0; n < NUM_NEIGHBORS; n++) {
          if (Xi == csp->neighbors[var][n]) {
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
          int item[2] = {var, Xi};
          enqueue(q, item);
        }
      }
    }
  }
  return 1;
}

int revise(CSP *csp, int Xi, int Xj) {
  int revised = 0; // false

  // Want to see how many values in Xi's domain satisfy the ruls of soduku
  int XiValue, XjValue, valXi, valXj, satisfied;

  for (valXi = 0; valXi < NUM_VALUES; valXi++) {
    // Initialize each time to ensure filled with 0's when calling count()
    int constraintSatisfied[NUM_VALUES] = {0,0,0,0,0,0,0,0,0};

    XiValue = csp->curr_domains[Xi][valXi];
    if (XiValue == 0) { continue; }
    // Check each value in Xi's domain against every value in Xj's domain
    for (valXj = 0; valXj < NUM_VALUES; valXj++) {
      XjValue = csp->curr_domains[Xj][valXj];
      if (XjValue == 0) { continue; }

      // If XiValue is in Xi's domain (not 0) and XjValue is in Xj's domain
      // check if ((Xi != Xj) && (XiValue == XjValue)) and set entry equal
      // to result of satisfaction check (0 if false, 1 if true)
      satisfied = csp->constraint(Xi, XiValue, Xj, XjValue);
      constraintSatisfied[valXj] = satisfied;
    }

    if (count(constraintSatisfied) != 0) {
      prune(csp, Xi, XiValue);
      revised = 1;
    }
  }
  return revised;
}

int *backtracking_search(CSP *csp) {
  return backtrack(csp);
}

int *backtrack(CSP *csp) {
  if (goal_test(csp) != 0) { return csp->assignment; }

  int val, variable = select_unassigned_variable(csp);
  for (val = 0; val < NUM_VALUES; val++) {
    int conflicts = nconflicts(csp, variable, val);

    if (conflicts == 0) {
      assign(csp, variable, val);
      suppose(csp, variable, val);
      int *result = backtrack(csp);

      if (result != NULL) { return result; }
      else { unassign(csp, variable); }
    }
  }
  return NULL;
}

int select_unassigned_variable(CSP *csp) {
  int var;
  for (var = 0; var < NUM_SLOTS; var++) {
    if (csp->assignment[var] == 0) { return var; } // Return first unassigned variable
  }
  return -1; // All variables assigned a value
}


void get_queue(CSP *csp, Queue *q, int variable) {
  // We want to find each variable-variable pair that is in the same row and/or
  // column, which will help us check our possible solution against our constraint
  int checkNeighbors;
  for (checkNeighbors = 0; checkNeighbors < NUM_NEIGHBORS; checkNeighbors++) {
    int neighbor = csp->neighbors[variable][checkNeighbors];
    if (neighbor  == 1) {
      int tuple[2] = {variable, neighbor};
      enqueue(q, tuple);
    }
  }
}

 int main() {
  CSP *csp = initCSP();
  Queue *q = initQueue(NULL);
  clock_t start;

  printf("Starting the solve with AC3 procedure...\n");
  start = clock();
  AC3(q, csp);
  printf("AC3 solved the board in %f seconds.\n\n", difftime(start, clock()));
  display(csp);
  destroyCSP(csp);
  destroyQueue(q);

  csp = initCSP();
  printf("Starting the solve with Backtracking procedure...\n");
  start = clock();
  backtracking_search(csp);
  printf("Backtracking solved the board in %f seconds.\n\n", difftime(start, clock()));
  display(csp);
  destroyCSP(csp);

  return 0;
}

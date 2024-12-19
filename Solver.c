#include "Solver.h"
#include "CSP.c"
#include "datastructs.c"
#include <stdlib.h>

#define NUM_VALUES 9
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#define NUM_NEIGHBORS (2 * (NUM_VALUES - 1))

int AC3(Queue *q, CSP *csp) {
  if (q->currSize == 0) {
    int var;
    for (var = 0; var < NUM_SLOTS; var++) {
      get_queue(csp, q, var);
    } 
  }

  int X[2] = {0,0};
  while (q->head != q->tail) {
    dequeue(&q, X);

    if (revise(csp, X[0], X[1]) == 1) {
      if (count(csp->curr_domains[X[0]]) == 0) { return 0; }

      int domain, qIndex, isInQueue, nIndex, isNeighbor;
      int newTuple[2] = {0,0};
      for (domain = 0; domain < NUM_SLOTS; domain++) {
	// Don't enqueue a tuple consisting of the same variable ((0,0), (1,1), etc.)
	if (domain == X[1]) { continue; }

	// Don't enqueue a tuple consisting of non-neighboring variables (constraint doesn't apply)
	nIndex = 0, isNeighbor = 0;
        while (nIndex != NUM_NEIGHBORS && isNeighbor != 1) {
	  if (X[0] == csp->neighbors[domain][nIndex]) {
	    isNeighbor = 1;
	  }
	  nIndex++;
	}
	if (isNeighbor == 0) { continue; }

	// Don't enqueue the tuple exists in the queue already
	qIndex = 0, isInQueue = 0;
	while (qIndex != q->maxSize && isInQueue != 1) {
	  if (domain == q->tuples[qIndex][0] && X[0] == q->tuples[qIndex][1]) {
	    isInQueue = 1;
	  }
	  qIndex++;
	}
	if (isInQueue == 1) { continue; }

	newTuple[0] = domain, newTuple[1] = X[0];
	enqueue(&q, newTuple);
      } // End for loop      
    } // End revise
  } // End while
  
  return 1;
}

int revise(CSP *csp, int Xi, int Xj) {
  int revised = 0; // false

  // Want to see how many values in Xi's domain satisfy the rules of soduku
  int XiValue, XjValue, valXi, valXj, satisfied;

  int unsatisfied;
  for (valXi = 0; valXi < NUM_VALUES; valXi++) {
    unsatisfied = 0;

    // If valXi is not in the variable's current domain, constraint doesn't apply
    XiValue = csp->curr_domains[Xi][valXi];
    if (XiValue == 0) { continue; }
    
    // Check each value in Xi's domain against every value in Xj's domain
    for (valXj = 0; valXj < NUM_VALUES; valXj++) {
      XjValue = csp->curr_domains[Xj][valXj];
      if (XjValue == 0) { continue; }

      // Since Xi and Xj are neighbors (as per the enqueue process in AC3)
      // We check if the constraint is satisfied between and of their possible values
      // AKA, compare their current domains
      unsatisfied += csp->constraint(Xi, XiValue, Xj, XjValue);
    }

    // If the value being considered for variable Xi has any conflicts with
    // any of the possible values of Xj, remove value from Xi's current domain
    if (unsatisfied != 0) {
      prune(csp, Xi, XiValue);
      revised = 1;
    }
    
  } // End Xi domain loop
  
  return revised;
}

int *backtracking_search(CSP *csp) {
  return backtrack(csp);
}

int *backtrack(CSP *csp) {
  // If board is completed correctly, return assignments
  if (goal_test(csp) == 1) { return csp->assignment; }

  // If all variables are assigned, but board is incorrect
  // Look for the variable with the most conflicts
  int variable = select_unassigned_variable(csp);
  if (variable = -1) {
    int i, j, currConflicts;
    int mostConflicts[2]= {0,0};
    for (i = 0; i < NUM_SLOTS; i++) {
      // Check how many conflicts the current assignment has
      currConflicts = nconflicts(csp, i, csp->assignment[i]);
      if (currConflicts > mostConflicts[1]) {
	mostConflicts[0] = i, mostConflicts[1] = currConflicts;
      }
    } // End variable loop
    variable = mostConflicts[0];
  }

  int val, conflicts, *result;
  for (val = 0; val < NUM_VALUES; val++) {
    conflicts = nconflicts(csp, variable, val);

    if (conflicts == 0) {
      assign(csp, variable, val);
      suppose(csp, variable, val);
      result = backtrack(csp);

      if (result != NULL) { return result; }
      else { unassign(csp, variable); }
    } // End if no conflicts
  } // End for loop

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
  int nIndex, neighbor;
  int tuple[2] = {variable,0};
  for (nIndex = 0; nIndex < NUM_NEIGHBORS; nIndex++) {
    tuple[1] = csp->neighbors[variable][nIndex];
    enqueue(&q, tuple);
  }
}

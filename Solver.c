#include "Solver.h"
#include "CSP.c"
#include "datastructs.c"
#include <stdlib.h>

int AC3(Queue **q, CSP **csp) {
  if ((*q)->currSize == 0) {
    int var;
    for (var = 0; var < NUM_SLOTS; var++) {
      get_queue(*csp, q, var);
    }
  }

  printf("Queue has %d items.\n\n", (*q)->currSize);

  int *X = calloc(2, sizeof(int));
  int *newTuple = calloc(2, sizeof(int));
  int domain, qIndex, isInQueue, nIndex, isNeighbor;
  while ((*q)->currSize != 0) {
    dequeue(q, &X);

    // Check for constraint violations between first pair of neighbors
    if (revise(csp, X[0], X[1]) == 1) {
      if (count((*csp)->curr_domains[X[0]]) == 0) {
	printf("Found empty domain for variable %d with %d items left in queue!\n", X[0], (*q)->currSize);
	return 0;
      }

      for (domain = 0; domain < NUM_SLOTS; domain++) {
	// Don't enqueue a tuple consisting of the same variable ((0,0), (1,1), etc.)
	if (domain == X[0] || domain == X[1]) { continue; }

	// Don't enqueue a tuple consisting of non-neighboring variables (constraint doesn't apply)
	nIndex = 0, isNeighbor = 0;
        while (nIndex != NUM_NEIGHBORS && isNeighbor != 1) {
	  if (X[0] == (*csp)->neighbors[domain][nIndex]) {
	    isNeighbor = 1;
	  }
	  nIndex++;
	}
	if (isNeighbor == 0) { continue; }

	// Don't enqueue the tuple exists in the queue already
	qIndex = 0, isInQueue = 0;
	while (qIndex != (*q)->maxSize && isInQueue != 1) {
	  if (domain == (*q)->tuples[qIndex][0] && X[0] == (*q)->tuples[qIndex][1]) {
	    isInQueue = 1;
	  }
	  qIndex++;
	}
	if (isInQueue == 1) { continue; }

	newTuple[0] = domain, newTuple[1] = X[0];
	printf("(%d, %d) now enqueued\n", domain, X[0]);
	enqueue(q, &newTuple);
      } // End for loop
    } // End revise
    else { printf("No revisision for (%d, %d)!\n", X[0], X[1]); }
  } // End while

  free(X);
  X = NULL;
  free(newTuple);
  newTuple = NULL;

  // All variables have been assigned without conflict - now update csp's assignments
  infer_assignment(csp);

  return 1;
}

int revise(CSP **csp, int Xi, int Xj) {
  int revised = 0; // false
  // If Xi is not a variable, it cannot be unassigned its value
  if (isVariable(*csp, Xi) == 0) {
    printf("%d is not a variable!\n", Xi);
    return revised;
  }

  // Want to see which values in Xi's current domain satisfy the rules of Soduku
  int XiValue, XjValue, val, unsatisfied;

  for (val = 0; val < NUM_VALUES; val++) {
    // If Xj is not a variable, we need to use its assigned value
    // since all atomics have current domains of 0's
    if (isVariable(*csp, Xj) == 0) { XjValue = (*csp)->assignment[Xj]; }
    else { XjValue = (*csp)->curr_domains[Xj][val]; }

    // If val is not in Xj's current domain, there is no conflict
    if (XjValue == 0) {
      printf("%d is not in %d's domain for (%d, %d)!\n", XjValue, Xj, Xi, Xj);
      continue;
    }

    // If val is not in Xi's current domain, there is no conflict
    XiValue = (*csp)->curr_domains[Xi][val];
    if (XiValue == 0) {
      printf("%d is not in %d's domain for (%d, %d)!\n", XiValue, Xi, Xi, Xj);
      continue;
    }

    // Since Xi and Xj are neighbors (as per the enqueue process in AC3),
    // we check if the constraint is satisfied between their current domains
    unsatisfied = (*csp)->constraint(Xi, XiValue, Xj, XjValue);

    // If the values being considered for variable Xi have any conflicts,
    // remove the values from Xi's current domain
    if (unsatisfied != 0) {
      printf("Getting rid of %d from %d's domain due to conflict with %d.\n", XiValue, Xi, Xj);
      prune(csp, Xi, XiValue);
      revised = 1;
    }

  } // End Xi domain loop

  return revised;
}

int *backtracking_search(CSP **csp) {
  return backtrack(csp);
}

int *backtrack(CSP **csp) {
  // If board is completed correctly, return assignments
  if (goal_test(*csp) == 1) { return (*csp)->assignment; }

  // If all variables are assigned, but board is incorrect
  // Look for the variable with the most conflicts
  int variable = select_unassigned_variable(*csp);
  if (variable == -1) {
    int i, currConflicts;
    int mostConflicts[2]= {0,0};
    for (i = 0; i < NUM_SLOTS; i++) {
      // Check how many conflicts the current assignment has
      currConflicts = nconflicts(*csp, i, (*csp)->assignment[i]);
      if (currConflicts > mostConflicts[1]) {
	mostConflicts[0] = i, mostConflicts[1] = currConflicts;
      }
    } // End variable loop
    variable = mostConflicts[0];
  }

  int val, conflicts, *result;
  for (val = 0; val < NUM_VALUES; val++) {
    conflicts = nconflicts(*csp, variable, val);

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
  for (var = 0; var < csp->numVars; var++) {
    if (csp->assignment[csp->variables[var]] == 0) { return var; } // Return first unassigned variable
  }
  return -1; // All variables assigned a value
}


void get_queue(CSP *csp, Queue **q, int variable) {
  // We want to find each variable-variable pair that is in the same row and/or
  // column, which will help us check our possible solution against our constraint
  int nIndex;
  int *tuple = calloc(2, sizeof(int));
  tuple[1] = variable;
  for (nIndex = 0; nIndex < NUM_NEIGHBORS; nIndex++) {
    tuple[0] = csp->neighbors[variable][nIndex];
    enqueue(q, &tuple);
  }
  free(tuple);
  tuple = NULL;
}

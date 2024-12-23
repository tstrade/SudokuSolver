#include "Solver.h"
#include "CSP.c"
#include "datastructs.c"
#include <stdlib.h>

int bcount;

int AC3(Queue *q, CSP *csp) {
  if (q->currSize == 0) {
    int var;
    for (var = 0; var < NUM_SLOTS; var++) {
      get_queue(csp, q, var);
    }
  }

  int *X = calloc(2, sizeof(int));
  int domain, nIndex, isNeighbor;
  while (q->currSize != 0) {
    dequeue(q, X);

    // Check for constraint violations between first pair of neighbors
    if (revise(csp, X[0], X[1]) == 1) {
      if (count(csp->curr_domains[X[0]]) == 0) {
	printf("Found empty domain for variable %d with %d items left in queue!\n", X[0], q->currSize);
	return 0;
      }

      for (domain = 0; domain < NUM_SLOTS; domain++) {
	// Don't enqueue a tuple consisting of the same variable ((0,0), (1,1), etc.)
	if (domain == X[0]) { continue; }

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
        if (q->isInQueue[X[0]][domain] == 1) { continue; }

	enqueue(q, domain, X[0]);

      } // End for loop
    } // End revise
  } // End while

  free(X);
  X = NULL;

  infer_assignment(csp);

  return 1;
}

int revise(CSP *csp, int Xi, int Xj) {
  int revised = 0; // false

  // Want to see which values in Xi's current domain satisfy the rules of Soduku
  int XiValue, XjValue, valXi, valXj, satisfied;

  for (valXi = 0; valXi < NUM_VALUES; valXi++) {
    // If val is not in Xi's current domain, there is no conflict
    XiValue = csp->curr_domains[Xi][valXi];
    if (XiValue == 0) { continue; }

    satisfied = 0;
    for (valXj = 0; valXj < NUM_VALUES; valXj++) {
      XjValue = csp->curr_domains[Xj][valXj];
      // If val is not in Xj's current domain, there is no conflict
      if (XjValue == 0) { continue; }

      // Since Xi and Xj are neighbors (as per the enqueue process in AC3),
      // we check if the constraint is satisfied between their current domains
      satisfied += csp->constraint(Xi, XiValue, Xj, XjValue);

    } // End Xj domain loop

    // If the constraint is violated, remove the value from Xi's domain
    if (satisfied == 0) {
      prune(csp, Xi, XiValue);
      revised = 1;
    }
  } // End Xi domain loop

  return revised;
}
/*
int *backtracking_search(CSP *csp) {
  return backtrack(csp);
}

// Unsorted domain of 0: 1, 2, 3, 4, 5, 6, 7, 8, 9
// Sorted domain of 0:   4, 5, 1, 2, 3, 6, 7, 8, 9

void order_domain_values(CSP *csp, int variable, int **ordered_domain) {
  int i, numConflicts, leastConflicts, val = 0;

  while (val != NUM_VALUES) {
    for (i = 0; i < NUM_VALUES; i++) {

    }
  }
}

int *backtrack(CSP *csp) {
  printf("\n  Called backtrack %d times", ++bcount);
  // If board is completed correctly, return assignments
  if (goal_test(csp) == 1) { return csp->assignment; }

  // Select an unassigned variable with the smallest domain
  int variable = select_unassigned_variable(csp);
  printf("Next unassigned variable is %d\n", variable);

  int val, conflicts, *result;
  int ordered_domain[NUM_VALUES] = {0,0,0,0,0,0,0,0,0} ;

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
  int var, variable, smallestDomain, tempSize, domainSize = 0;
  for (var = 0; var < csp->numVars; var++) {
    variable = csp->variables[var];
    if (csp->assignment[variable] != 0) { continue; }
    tempSize = count(csp->curr_domains[variable]);

    if (domainSize == 0) {
      smallestDomain = variable;
      domainSize = tempSize;
    } else if (domainSize > tempSize) {
      smallestDomain = variable;
      domainSize = tempSize;
    }
  }
  return smallestDomain;
}
*/

void get_queue(CSP *csp, Queue *q, int variable) {
  // We want to find each variable-variable pair that is in the same row/column/box,
  // which will help us check our possible solution against our constraint
  int nIndex;
  for (nIndex = 0; nIndex < NUM_NEIGHBORS; nIndex++) {
    enqueue(q, csp->neighbors[variable][nIndex], variable);
  }
}

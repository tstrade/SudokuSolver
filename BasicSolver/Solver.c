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
  int domain, nIndex;
  while (q->currSize != 0) {
    dequeue(q, X);
    int Xi = X[0], Xj = X[1];

    // Check for constraint violations between first pair of neighbors
    if (revise(csp, Xi, Xj) == 1) {
      if (count(csp->curr_domains[Xi], NUM_VALUES) == 0) {
	      printf("Found empty domain for variable %d with %d items left in queue!\n", Xi, q->currSize);
	      return 0;
      }

      for (domain = 0; domain < NUM_SLOTS; domain++) {
        // Don't enqueue a tuple consisting of the same variable ((0,0), (1,1), etc.)
        if (domain == Xj) { continue; }

        // Don't enqueue a tuple consisting of non-neighboring variables (constraint doesn't apply)
        nIndex = 0;
        while (nIndex != NUM_NEIGHBORS) {
          if (Xi == csp->neighbors[domain][nIndex]) { goto is_neighbor; }
          nIndex++;
        }
        continue;

        is_neighbor:
        // Don't enqueue the tuple exists in the queue already
        if (q->isInQueue[domain][Xi] == 1) { continue; }

        enqueue(q, domain, Xi);
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

int *backtracking_search(CSP *csp) {
  return backtrack(csp);
}


int *backtrack(CSP *csp) {
  printf("\n  Called backtrack %d times\n", ++bcount);
  // If board is completed correctly, return assignments
  if (goal_test(csp)) { printf("Inferring assigment...\n"); infer_assignment(csp); return csp->assignment; }

  // Select an unassigned variable with the smallest domain
  int variable = select_unassigned_variable(csp);
  printf("\n  Next unassigned variable is %d\n", variable);

  int conflicts, *result;
  order_domain_values(csp, variable);

  for (int value = 0; value < NUM_VALUES; value++) {
    conflicts = nconflicts(csp, variable, value);

    if (!conflicts) {
      assign(csp, variable, value);
      suppose(csp, variable, value);
      result = backtrack(csp);

      if (result != NULL) { return result; }
      else { printf("Unassigning %d...\n", variable); unassign(csp, variable); }
    } // End if no conflicts
  } // End for loop

  printf("Backtracking failed... continue debugging :)\n");
  exit(EXIT_FAILURE);
  return NULL;
}

int select_unassigned_variable(CSP *csp) {
  int smallestDomain, size, domainSize = NUM_VALUES;
  for (int slot = 0; slot < NUM_SLOTS; slot++) {
    if (csp->assignment[slot] != 0) { continue; }
    if ((size = count(csp->curr_domains[slot], NUM_VALUES)) < domainSize) {
      smallestDomain = slot; domainSize = size;
    }
  }
  return smallestDomain;
}

void order_domain_values(CSP *csp, int variable) {
  for (int i = 0; i < NUM_VALUES - 1; i++) {
    for (int j = 0; j < NUM_VALUES - i - 1; j++) {
      if (nconflicts(csp, variable, csp->curr_domains[variable][j]) > nconflicts(csp, variable, csp->curr_domains[variable][j + 1])) {
	swap(&(csp->curr_domains[variable][j]), &(csp->curr_domains[variable][j + 1]));
      }
    }
  }
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void get_queue(CSP *csp, Queue *q, int variable) {
  // We want to find each variable-variable pair that is in the same row/column/box,
  // which will help us check our possible solution against our constraint
  int nIndex;
  for (nIndex = 0; nIndex < NUM_NEIGHBORS; nIndex++) {
    enqueue(q, csp->neighbors[variable][nIndex], variable);
  }
}

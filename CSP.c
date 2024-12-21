#include "CSP.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_VALUES 9
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#define NUM_NEIGHBORS ((NUM_VALUES - 1) + (2 * (NUM_VALUES - (NUM_VALUES/3))))
#define NUM_BOX (NUM_VALUES / 3)

int **actionOptions;

struct CSP {
  int numVars;
  int *variables;
  int **domains;
  int **neighbors;
  int nassigns;
  int **curr_domains;
  int **removals;
  int *inference;
  int *assignment;
  int *initialAssignments;
  int (*constraint)(int, int, int, int);
};

int sortNeighbors(const void *a, const void *b) {
  return (*(int*)a - *(int*)b);
}

void checkNULL(void *ptr) {
  if (ptr == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
}

void initNeighbors(CSP *csp) {
  int var, val, row, col, box_start_row, box_start_col, nIndex, r, c;
  int *added, added_idx;

  for (var = 0; var < NUM_SLOTS; var++) {
      added = calloc(NUM_SLOTS, sizeof(int));

      row = getRow(var);
      col = getCol(var);

      box_start_row = (row / NUM_BOX) * NUM_BOX;
      box_start_col = (col / NUM_BOX) * NUM_BOX;

      nIndex = 0;

      // Add row neighbors
        for (val = 0; val < NUM_VALUES; val++) {
            added_idx = row * NUM_VALUES + val;
            if (added_idx != var && !added[added_idx]) {
	        csp->neighbors[var][nIndex] = added_idx;
                added[added_idx] = 1;
                nIndex++;
            }
        }

      // Add column neighbors
        for (val = 0; val < NUM_VALUES; val++) {
            added_idx = val * NUM_VALUES + col;
            if (added_idx != var && !added[added_idx]) {
	        csp->neighbors[var][nIndex] = added_idx;
                added[added_idx] = 1;
                nIndex++;
            }
        }

      // Add box neighbors
        for (r = box_start_row; r < box_start_row + NUM_BOX; r++) {
            for (c = box_start_col; c < box_start_col + NUM_BOX; c++) {
                added_idx = r * NUM_VALUES + c;
                if (added_idx != var && !added[added_idx]) {
		    csp->neighbors[var][nIndex] = added_idx;
                    added[added_idx] = 1;
                    nIndex++;
                }
            }
        }

        free(added);
        added = NULL;
    }
}

CSP *initCSP(CSP *self) {
  self = malloc(sizeof(CSP));
  // Return NULL if not enough memory for the CSP structure
  checkNULL((void *)self);

  int var, val;

  // For each variable, there is a list of possible entries ranging from 1 to 9
  self->domains = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->domains);

  for (var = 0; var < NUM_SLOTS; var++) {
    self->domains[var] = malloc(NUM_VALUES * sizeof(int));
    checkNULL((void *)self->domains[var]);

    // Fill domains with appropriate range of values
    for (val = 0; val < NUM_VALUES; val++) {
      self->domains[var][val] = val + 1;
    }
  }

  // For each variable, there is a list of variables that adhere to the constraints
  self->neighbors = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->neighbors);

  for (var = 0; var < NUM_SLOTS; var++) {
    self->neighbors[var] = malloc(NUM_NEIGHBORS * sizeof(int));
    checkNULL((void *)self->neighbors[var]);
  }

  // As we make suppositions about variable assignments, we want to be
  // able to backtrack in case the supposition ends up incorrect
  // This "dict" will be cleared each time suppose() is called
  self->removals = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->removals);

  for (val = 0; val < NUM_SLOTS; val++) {
    self->removals[val] = calloc(NUM_VALUES, sizeof(int));
    checkNULL((void *)self->removals[val]);
  }

  self->inference = calloc(NUM_SLOTS, sizeof(int));
  checkNULL((void *)self->inference);

  self->assignment = calloc(NUM_SLOTS, sizeof(int));
  checkNULL((void *)self->inference);

  // Assign the Soduku Rules to the CSP constaint function
  self->constraint = Soduku_Constraint;

  self->nassigns = 0;

  self->numVars = 0;

  self->curr_domains = NULL;

  initNeighbors(self);

  return self;
}

void assign(CSP *self, int variable, int value) {
  self->assignment[variable] = value;
  self->nassigns++;
}

void unassign(CSP *self, int variable) {
  self->assignment[variable] = 0;
}


int nconflicts(CSP *self, int variable, int value){
  int n, conflicts = 0;

  for (n = 0; n < NUM_NEIGHBORS; n++) {
    int neighbor = self->neighbors[variable][n];
    conflicts += self->constraint(variable, value, neighbor, self->assignment[neighbor]);
  }
  return conflicts;
}

void display(CSP *self) {
  int slot;
  char *assignment = malloc(NUM_SLOTS * sizeof(char));

  for (slot = 0; slot < NUM_SLOTS; slot++) {
    assignment[slot] = self->assignment[slot] + 48;
    /*
    if (assignment[slot] == '0') {
      assignment[slot] = '.';
    }
    */
  }

  printf("\n\n   ---------------------------------\n");

  for (slot = 0; slot < NUM_SLOTS; slot += 9) {
    printf("  | %c  %c  %c  |  %c  %c  %c  |  %c  %c  %c |\n",
    assignment[slot], assignment[slot + 1], assignment[slot + 2],
    assignment[slot + 3], assignment[slot + 4], assignment[slot + 5],
    assignment[slot + 6], assignment[slot + 7], assignment[slot + 8]);

    if (slot == NUM_VALUES * 2 || slot == NUM_VALUES * 5) {
      // Only need two dividers (board is in thirds)
      printf("   ----------+-----------+----------\n");
    }
  }

  printf("   ---------------------------------\n\n");

  free(assignment);
  assignment = NULL;
}

int **actions(CSP *self) {
  int varValTuple;

  if (actionOptions == NULL) {
    actionOptions = malloc(NUM_VALUES * sizeof(int *));
    checkNULL((void *)actionOptions);

    for (varValTuple = 0; varValTuple < NUM_VALUES; varValTuple++) {
      actionOptions[varValTuple] = calloc(2, sizeof(int));
      checkNULL((void *)actionOptions[varValTuple]);
    }
  }

  // All assignments have been made - no actions to be taken
  if (count(self->assignment) == NUM_SLOTS) {
    return NULL;
  }
  else { // Check for applicable actions and return list (might need to be a queue?)
    int var, variable;
    for (var = 0; var < NUM_SLOTS; var++) {
      if (self->assignment[var] == 0) {
        variable = self->variables[var];
        break;
      }
    }

    // Collect non-conlicting possible values and return list
    int value;
    for (varValTuple = 0; varValTuple < NUM_VALUES; varValTuple++) {
      value = self->domains[variable][varValTuple];
      if (nconflicts(self, variable, value) == 0) {
        actionOptions[varValTuple][0] = variable;
        actionOptions[varValTuple][1] = value;
      }
    }
    return actionOptions;
  }
}

int *result(CSP *self, int *action) {
  self->assignment[action[0]] = action[1];
  return self->assignment;
}

int goal_test(CSP *self) {
  int var;

  // If all variables haven't been assigned, the game is not complete
  if (count(self->assignment) != NUM_SLOTS) { return 0; }

  for (var = 0; var < NUM_SLOTS; var++) {
    // If any (Variable, Value) pair has a conflict, the game is not complete
    if (nconflicts(self, var, self->assignment[var]) != 0) { return 0; }
  }

  // Else, the game is complete!
  return 1;
}

void support_pruning(CSP *self) {
  if (self->curr_domains != NULL) { return; }
  // printf("Establishing current domains...\n");
  int var, val;

  self->curr_domains = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->curr_domains);

  for (var = 0; var < NUM_SLOTS; var++) {
    self->curr_domains[var] = calloc(NUM_VALUES, sizeof(int));
    checkNULL((void *)self->curr_domains[var]);

    for (val = 0; val < NUM_VALUES; val++) {
      if (isVariable(self, var)) {
	self->curr_domains[var][val] = self->domains[var][val];
      }
    } // End values loop
  } // End slots loop
  // printf("Pruning now supported!\n");

  // Get rid of domain values if neighbor is already assigned
  int nIndex, neighbor;
  for (var = 0; var < self->numVars; var++) {
    for (val = 0; val < NUM_VALUES; val++) {
      for (nIndex = 0; nIndex < NUM_NEIGHBORS; nIndex++) {
	neighbor = self->neighbors[self->variables[var]][nIndex];
	if (self->curr_domains[self->variables[var]][val] == self->assignment[neighbor]) {
	  prune(self, var, self->assignment[neighbor]);
	}
      } // End neighbor loop
    } // End value loop
  } // End variable loop
}


void suppose(CSP *self, int variable, int value) {
  // Setup curr_domains if necessary
  support_pruning(self);

  int val, checkValue;
  for (val = 0; val < NUM_VALUES; val++) {
    checkValue = self->curr_domains[variable][val];
    if (checkValue != value) {
      // Remove any values that do not align with the supposition
      self->removals[variable][val] = checkValue;
      self->curr_domains[variable][val] = 0;
    } else {
      // Make sure supposed value is not set in removed "dict"
      self->removals[variable][val] = 0;
      self->curr_domains[variable][val] = value; // Redundant, but for consistency's sake
    }
  } // End values loop
}

void prune(CSP *self, int variable, int value) {
  self->curr_domains[variable][value - 1] = 0;
  self->removals[variable][value - 1] = value;
}

void infer_assignment(CSP *self) {
  // Setup curr_domains if necessary
  support_pruning(self);

  int possibleInference, i, var, val;
  for (i = 0; i < self->numVars; i++) {
    var = self->variables[i];

    for (val = 0; val < NUM_VALUES; val++) {
      if (self->curr_domains[var][val] != 0) {
        possibleInference = self->curr_domains[var][val];
      }
    }

    if (count(self->curr_domains[var]) == 1) {
      self->assignment[var] = possibleInference;
    } else {
      self->assignment[var] = 0;
    }
  }
}


void restore(CSP *self) {
  int i, var, val;
  for (i = 0; i < NUM_SLOTS; i++) {
    var = self->variables[i];
    for (val = 0; val < NUM_VALUES; val++) {
      if (self->removals[var][val] != 0) {
        self->curr_domains[var][val] = val + 1;
        self->removals[var][val] = 0;
      }
    }
  }
}

int Soduku_Constraint(int varA, int valA, int varB, int valB) {
  return ((varA != varB) && (valA == valB));
}

int count(int *seq) {
  int count = 0;
  for (int i = 0; i < NUM_VALUES; i++) {
    if (seq[i] != 0) { count++; }
  } // End values loop
  return count;
}

int getRow(int variable) {
  return variable / NUM_VALUES;
}

int getCol(int variable) {
  return variable % NUM_VALUES;
}

int isVariable(CSP *csp, int variable) {
  int var;
  for (var = 0; var < csp->numVars; var++) {
    if (variable == csp->variables[var]) { return 1; }
  }
  return 0;
}

void destroyCSP(CSP *self) {
  free(self->variables);

  int i;
  for (i = 0; i < NUM_SLOTS; i++) {
    free(self->domains[i]);
    free(self->neighbors[i]);
    free(self->curr_domains[i]);
    free(self->removals[i]);
  }
  for (i = 0; i < NUM_VALUES; i++) {
    free(actionOptions[i]);
  }

  free(actionOptions);
  actionOptions = NULL;
  free(self->removals);
  free(self->domains);
  free(self->neighbors);
  free(self->curr_domains);
  free(self->inference);
  free(self->assignment);

  free(self);
  self = NULL;
}

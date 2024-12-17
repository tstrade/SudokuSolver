#include "CSP.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_VALUES 9
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#define NUM_NEIGHBORS (2 * (NUM_VALUES - 1))

extern int **actionOptions;

struct CSP {
  int *variables;
  int **domains;
  int **neighbors;
  int nassigns;
  int **curr_domains;
  int **removals;
  int *inference;
  int *assignment;
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
  int var, potentialNeighbor, neighborX, row;
  // For each variable (slot on the board), we want to know what other
  // variables (slots) are in their row / column (macros allow this to be adjustable size)
  for (var = 0; var < NUM_SLOTS; var++) {
    neighborX = 0, row = 0;
    potentialNeighbor = var - (var % NUM_VALUES);

    while (neighborX < NUM_NEIGHBORS) {
      if (potentialNeighbor != var) {
        csp->neighbors[var][neighborX] = potentialNeighbor;
        neighborX++;
      }
      csp->neighbors[var][neighborX] = (var % NUM_VALUES) + (NUM_VALUES * ++row);
      neighborX++;
      potentialNeighbor++;
    }
    // Not necessary, but sorting can help with visuals later on
    qsort(csp->neighbors[var], NUM_NEIGHBORS, sizeof(int), sortNeighbors);
  }
}

CSP *initCSP() {
  CSP *self = malloc(sizeof(CSP));
  // Return NULL if not enough memory for the CSP structure
  checkNULL((void *)self);

  int var, val;

  // List of atomic variables - for Soduku, it will be the
  // indexes of the 1D array representation of the board
  self->variables = malloc(NUM_SLOTS * sizeof(int));
  checkNULL((void *)self->variables);

  for (var = 0; var < NUM_SLOTS; i++) { self->variables[var] = var; }

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
  // 8 neighbors in the same row + 8 neighbors in the same column = 16 neighbors
  self->neighbors = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->neighbors);

  for (val = 0; val < NUM_SLOTS; val++) {
    self->neighbors[var] = malloc(NUM_NEIGHBORS * sizeof(int));
    checkNULL((void *)self->neighbors[var]);
  }

  // As we make suppositions about variable assignments, we want to be
  // able to backtrack in case the supposition ends up incorrect
  // This "dict" will be cleared each time suppose() is called
  self->removals = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->removals);

  for (val = 0; val < NUM_VALUES; val++) {
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
  int slot, value;

  for (slot = 0; slot <= (NUM_SLOTS - NUM_VALUES); slot++) {
    value = self->assignment[slot];
    if (slot % (NUM_SLOTS / 3) == 0) { // Only need two dividers (board is in thirds)
      printf(" %d |", value);
    }
    if ((slot % NUM_VALUES) == (NUM_VALUES - 1)) {
      // end of row
      printf(" %d\n", value);
      // Only need two dividers (board is in thirds)
      printf("----------+-----------+----------\n");
    } else {
      printf(" %d .", value);
    }
  }
  printf("\n\n\n");
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

  int var, val;

  self->curr_domains = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->curr_domains);

  for (var = 0; var < NUM_SLOTS; var++) {
    self->curr_domains[var] = malloc(NUM_VALUES * sizeof(int));
    checkNULL((void *)self->curr_domains[var]);

    for (val = 0; val < NUM_VALUES; val++) {
      self->curr_domains[var][val] = self->domains[var][val];
    } // End values loop
  } // End slots loop
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
  self->curr_domains[variable][value] = 0;
  self->removals[variable][value] = value;
}

void infer_assignment(CSP *self) {
  // Setup curr_domains if necessary
  support_pruning(self);

  int i, j, possibleInference;
  for (i = 0; i < NUM_SLOTS; i++) {
    for (j = 0; j < NUM_VALUES; j++) {
      if (self->curr_domains[i][j] != 0) {
        possibleInference = self->curr_domains[i][j];
      }
    }

    if (count(self->curr_domains[i]) == 1) {
      self->inference[i] = possibleInference;
    } else {
      self->inference[i] = 0;
    }
  }
}


void restore(CSP *self) {
  int var, val;
  for (var = 0; var < NUM_SLOTS; var++) {
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
  int i, count = 0;
  for (i = 0; i < NUM_VALUES; i++) {
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

void destoryCSP(CSP *self) {
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
  free(self->removals);
  free(self->domains);
  free(self->neighbors);
  free(self->curr_domains);
  free(self->inference);
  free(self->assignment);

  free(self);
}

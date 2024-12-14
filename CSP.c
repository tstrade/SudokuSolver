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

  int i, j;

  // List of atomic variables - for Soduku, it will be the
  // indexes of the 1D array representation of the board
  self->variables = malloc(NUM_SLOTS * sizeof(int));
  checkNULL((void *)self->variables);

  for (i = 0; i < NUM_SLOTS; i++) { self->variables[i] = i; }

  // For each variable, there is a list of possible entries ranging from 1 to 9
  self->domains = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->domains);

  for (i = 0; i < NUM_SLOTS; i++) {
    self->domains[i] = malloc(NUM_VALUES * sizeof(int));
    checkNULL((void *)self->domains[i]);

    // Fill domains with appropriate range of values
    for (j = 0; j < NUM_VALUES; j++) {
      self->domains[i][j] = j + 1;
    }
  }

  // For each variable, there is a list of variables that adhere to the constraints
  // 8 neighbors in the same row + 8 neighbors in the same column = 16 neighbors
  self->neighbors = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->neighbors);

  for (i = 0; i < NUM_SLOTS; i++) {
    self->neighbors[i] = malloc(NUM_NEIGHBORS * sizeof(int));
    checkNULL((void *)self->neighbors[i]);
  }

  // As we make suppositions about variable assignments, we want to be
  // able to backtrack in case the supposition ends up incorrect
  // This "dict" will be cleared each time suppose() is called
  self->removals = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->removals);

  for (i = 0; i < NUM_VALUES; i++) {
    self->removals[i] = calloc(NUM_VALUES, sizeof(int));
    checkNULL((void *)self->removals[i]);
  }

  self->inference = calloc(NUM_SLOTS, sizeof(int));
  checkNULL((void *)self->inference);

  // Assign the Soduku Rules to the CSP constaint function
  self->constraint = Soduku_Constraint;

  self->nassigns = 0;

  initNeighbors(self);

  return self;
}

void assign(CSP *self, int variable, int value, int *assignment) {
  assignment[variable] = value;
  self->nassigns++;
}

void unassign(int variable, int *assignment) {
  assignment[variable] = 0;
}


int nconflicts(CSP *self, int variable, int value, int *assignment){
  int n, conflicts = 0;

  for (n = 0; n < NUM_NEIGHBORS; n++) {
    int neighbor = self->neighbors[variable][n];
    conflicts += self->constraint(variable, value, neighbor, assignment[neighbor]);
  }
  return conflicts;
}

void display(int *assignment) {
  int slot, value;

  for (slot = 0; slot <= (NUM_SLOTS - NUM_VALUES); slot++) {
    value = assignment[slot];
    if (slot % (NUM_SLOTS / 3) == 0) { // Only need two dividers (board is in thirds)
      printf(" %d |", value);
    }
    if ((slot % NUM_VALUES) == (NUM_VALUES - 1)) {
      // end of row
      printf(" %d\n", assignment[slot]);
      // Only need two dividers (board is in thirds)
      printf("----------+-----------+----------\n");
    } else {
      printf(" %d .", value);
    }
  }
  printf("\n\n\n");
}

int **actions(CSP *self, int *state) {
  int stateLength = 0, i;

  if (actionOptions == NULL) {
    actionOptions = malloc(NUM_VALUES * sizeof(int *));
    checkNULL((void *)actionOptions);

    for (i = 0; i < NUM_VALUES; i++) {
      actionOptions[i] = calloc(2, sizeof(int));
      checkNULL((void *)actionOptions[i]);
    }
  }

  // Check if all assignments have been made
  for (i = 0; i < NUM_VALUES; i++) {
    if (state[i] != 0) { stateLength++; }
  }

  // All assignments have been made - no actions to be taken
  if (stateLength == NUM_SLOTS) {
    return NULL;
  }
  else { // Check for applicable actions and return list (might need to be a queue?)
    int variable;
    for (i = 0; i < NUM_SLOTS; i++) {
      if (state[i] == 0) {
        variable = self->variables[i];
        break;
      }
    }

    // Collect non-conlicting possible values and return list
    int value;
    for (i = 0; i < NUM_VALUES; i++) {
      value = self->domains[variable][i];
      if (nconflicts(self, variable, value, state) == 0) {
        actionOptions[i][0] = variable;
        actionOptions[i][1] = value;
      }
    }
    return actionOptions;
  }
}

int *result(int *state, int *action) {
  state[action[0]] = action[1];
  return state;
}

int goal_test(CSP *self, int *state) {
  int variablesAssigned = 0, i;

  for (i = 0; i < NUM_SLOTS; i++) {
    if (state[i] != 0) { variablesAssigned++; }
  }
  // If all variables haven't been assigned, the game is not complete
  if (variablesAssigned != NUM_SLOTS) { return 0; }

  for (i = 0; i < NUM_SLOTS; i++) {
    // If any (Variable, Value) pair has a conflict, the game is not complete
    if (nconflicts(self, i, state[i], state) != 0) { return 0; }
  }

  // Else, the game is complete!
  return 1;
}

void support_pruning(CSP *self) {
  if (self->curr_domains != NULL) { return; }

  int i, j;

  self->curr_domains = malloc(NUM_SLOTS * sizeof(int *));
  checkNULL((void *)self->curr_domains);

  for (i = 0; i < NUM_SLOTS; i++) {
    self->curr_domains[i] = malloc(NUM_VALUES * sizeof(int));
    checkNULL((void *)self->curr_domains[i]);

    for (j = 0; j < NUM_VALUES; j++) {
      self->curr_domains[i][j] = self->domains[i][j];
    } // End values loop
  } // End slots loop
}


void suppose(CSP *self, int variable, int value) {
  // Setup curr_domains if necessary
  support_pruning(self);

  int i, checkValue;
    for (i = 0; i < NUM_VALUES; i++) {
      checkValue = self->curr_domains[variable][i];
      if (checkValue != value) {
        // Remove any values that do not align with the supposition
        self->removals[variable][i] = checkValue;
        self->curr_domains[variable][i] = 0;
      } else {
        // Make sure supposed value is not set in removed "dict"
        self->removals[variable][i] = 0;
        self->curr_domains[variable][i] = value; // Redundant, but for consistency's sake
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

  int i, j, domainLength, possibleInference;
  for (i = 0; i < NUM_SLOTS; i++) {
    domainLength = 0;
    for (j = 0; j < NUM_VALUES; j++) {
      if (self->curr_domains[i][j] != 0) {
        domainLength++;
        possibleInference = self->curr_domains[i][j];
      }
    }

    if (domainLength == 1) {
      self->inference[i] = possibleInference;
    } else {
      self->inference[i] = 0;
    }
  }
}


void restore(CSP *self, int **removals) {
  int var, val;
  for (var = 0; var < NUM_SLOTS; var++) {
    for (val = 0; val < NUM_VALUES; val++) {
      if (removals[var][val] != 0) {
        self->curr_domains[var][val] = val + 1;
      }
    }
  }
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

  free(self);
}

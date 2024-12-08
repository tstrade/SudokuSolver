#include "CSP.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_SLOTS 81
#define NUM_VALUES 9
#define NUM_NEIGHBORS 16

struct CSP {
  int *variables;
  int **domains;
  int **neighbors;
  int nassigns;
  int **curr_domains;
  int (*constraint)(int, int, int, int);
};

CSP *initCSP() {
  CSP *self = malloc(sizeof(CSP));
  // Return NULL if not enough memory for the CSP structure
  if (self == NULL) { return NULL; }

  // List of atomic variables - for Soduku, it will be the
  // indexes of the 1D array representation of the board
  self->variables = malloc(NUM_SLOTS * sizeof(int));
  if (self->variables == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) { self->variables[i] = i; }

  // For each variable, there is a list of possible entries ranging from 1 to 9
  self->domains = malloc(NUM_SLOTS * sizeof(int *));
  if (self->domains == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) {
    self->domains[i] = malloc(NUM_VALUES * sizeof(int));
    if (self->domains[i] == NULL) { return NULL; }
  }

  // For each variable, there is a list of variables that adhere to the constraints
  // 8 neighbors in the same row + 8 neighbors in the same column = 16 neighbors
  self->neighbors = malloc(NUM_SLOTS * sizeof(int *));
  if (self->neighbors == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) {
    self->neighbors[i] = malloc(NUM_NEIGHBORS * sizeof(int));
    if (self->neighbors == NULL) { return NULL; }
  }

  // Assign the Soduku Rules to the CSP constaint function
  self->constraint = Soduku_Constraint;

  self->nassigns = 0;

  self->curr_domains = malloc(NUM_SLOTS * sizeof(int *));
  if (self->curr_domains == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) {
    self->curr_domains[i] = malloc(NUM_VALUES * sizeof(int));
    if (self->domains[i] == NULL) { return NULL; }
  }

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
  int conflicts = 0;
  if (value == 0 || variable > 80 || variable < 0 || self == NULL || assignment == NULL) {
    return conflicts;
  }

  int n;
  for (n = 0; n < NUM_NEIGHBORS; n++) {
    int neighbor = self->neighbors[variable][n];
    conflicts += self->constraint(variable, value, neighbor, assignment[neighbor]);
  }
  return conflicts;
}


void display(CSP *self, int *assignment);

int **actions(CSP *self, int *state);

int *result(int *state, int *action);

int goal_test(CSP *self, int *state);

void support_pruning(CSP *self);


int **suppose(CSP *self, int variable, int value);


void prune(CSP *self, int variable, int value, int **removals);

int *infer_assignment(CSP *self) {
  support_pruning(self);
  int *inferred = malloc(NUM_SLOTS * sizeof(int));
  if (inferred == NULL) { return NULL; }
  // return {var: self.curr_domains[var][0] for var in self.variables if 1 == len(self.curr_domains[var])}
  return inferred;
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

  for (int i = 0; i < NUM_SLOTS; i++) {
    free(self->domains[i]);
    free(self->neighbors[i]);
    free(self->curr_domains[i]);
  }
  free(self->domains);
  free(self->neighbors);
  free(self->curr_domains);

  free(self);
}

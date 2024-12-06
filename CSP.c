#include "CSP.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_SLOTS 81
#define NUM_VALUES 9
#define NUM_NEIGHBORS 16

struct CSP {
  int* variables;
  int** domains;
  int** neighbors;
  int nassigns;
  int (*constraint)(int, int, int, int);
  void (*assign)(CSP *self, int variable, int value, int *assignment);
  void (*unassign)(CSP *self, int variable, int *assignment);
  int (*nconflicts)(CSP *self, int variable, int value, int *assignment);
  void (*display)(CSP *self, int *assignment);
  int** (*actions)(CSP *self, int *state);
  int* (*result)(CSP *self, int *state, int *action);
  int (*goal_test)(CSP *self, int *state);
  void (*support_pruning)(CSP *self);
  int** (*suppose)(CSP *self, int variable, int value);
  void (*prune)(CSP *self, int variable, int value, int **removals);
  void (*restore)(CSP *self, int **removals);
};

CSP *__init__() {
  CSP *self = malloc(sizeof(CSP));
  // Return NULL if not enough memory for the CSP structure
  if (self == NULL) { return NULL; }

  // List of atomic variables - for Soduku, it will be the
  // indexes of the 1D array representation of the board
  self->variables = (int *)malloc(NUM_SLOTS * sizeof(int));
  if (self->variables == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) { self->variables[i] = i; }

  // For each variable, there is a list of possible entries ranging from 1 to 9
  self->domains = (int **)malloc(NUM_SLOTS * sizeof(int *));
  if (self->domains == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) {
    self->domains[i] = (int *)malloc(NUM_VALUES * sizeof(int));
    if (self->domains[i] == NULL) { return NULL; }
  }

  // For each variable, there is a list of variables that adhere to the constraints
  // 8 neighbors in the same row + 8 neighbors in the same column = 16 neighbors
  self->neighbors = (int **)malloc(NUM_SLOTS * sizeof(int *));
  if (self->neighbors == NULL) { return NULL; }
  for (int i = 0; i < NUM_SLOTS; i++) {
    self->neighbors[i] = (int *)malloc(NUM_NEIGHBORS * sizeof(int));
    if (self->neighbors == NULL) { return NULL; }
  }

  // Assign the Soduku Rules to the CSP constaint function
  self->constraint = Soduku_Constraint;

  self->nassigns = 0;

  return self;
}

void assign(CSP *self, int variable, int value, int *assignment) {
  assignment[variable] = value;
  self->nassigns++;
}

void unassign(CSP *self, int variable, int *assignment) {
  assignment[variable] = -1;
}


int nconflicts(CSP *self, int variable, int value, int *assignment);
void display(CSP *self, int *assignment);
int **actions(CSP *self, int *state);
int *result(CSP *self, int *state, int *action);
int goal_test(CSP *self, int *state);
void support_pruning(CSP *self);
int **suppose(CSP *self, int variable, int value);
void prune(CSP *self, int variable, int value, int **removals);
void restore(CSP *self, int **removals);

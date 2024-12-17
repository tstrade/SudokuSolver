#include "CSP.c"
#include "Soduku.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_VALUES 9
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#define NUM_NEIGHBORS (2 * (NUM_VALUES - 1))

struct Soduku {
  CSP *csp;
  void (*showVars)(Soduku *self);
  void (*showCurrentDomains)(Soduku *self);
  void (*showVariableDomain)(Soduku *self, int variable);
  void (*showRemovals)(Soduku *self);
  void (*destroySoduku)(Soduku *self);
};

Soduku *initBoard(char *initialPositions) {
  Soduku *self = malloc(sizeof(Soduku));
  self->csp = initCSP();
  support_pruning(self->csp);

  int variable;
  for (variable = 0; variable < NUM_SLOTS; variable++) {
    if (initialPositions[variable] == '.') {
      initialPositions[variable] = '0';
    }
    self->csp->assignment[variable] = (int)initialPositions[variable] - 48;
  }

  return self;
}

void showVars(Soduku *self) {
  int variable;
  printf("Variables: ");
  for (variable = 0; variable < NUM_SLOTS; variable++) {
    printf("%d  ", self->csp->variables[variable]);
  }
}

void showCurrentDomains(Soduku *self) {
  int variable;
  printf("Current variable domains are: \n");
  for (variable = 0; variable < NUM_SLOTS; variable++) {
    showVariableDomain(self, variable);
  }
}

void showVariableDomain(Soduku *self, int variable) {
  int values;
  printf("Variable %d current domains: ", variable);
  for (values = 0; values < NUM_VALUES; values++) {
    if (self->csp->curr_domains[variable][values] != 0) {
      printf("%d  ", self->csp->curr_domains[variable][values]);
    }
  }
  printf("\n");
}

void showRemovals(Soduku *self) {
  int variable, values;
  printf("Current removed values for variable: ");
  for (variable = 0; variable < NUM_SLOTS; variable++) {
    printf("%d: ", self->csp->variables[variable]);
    for (values = 0; values < NUM_VALUES; values++) {
      if (self->csp->removals[variable][values] != 0) {
        printf("%d  ", self->csp->removals[variable][values]);
      }
    }
    printf("\n");
  }
}

void destroySoduku(Soduku *self) {
  destroyCSP(self->csp);
  free(self);
}

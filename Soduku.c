#include "Solver.h"
#include "CSP.h"
#include "datastructs.h"
#include "Solver.c"
#include "Soduku.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define NUM_VALUES 9
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#define NUM_NEIGHBORS (2 * (NUM_VALUES - 1))

struct Soduku {
  CSP *csp;
  void (*showVars)(Soduku *self);
  void (*showCurrentDomains)(Soduku *self);
  void (*showVariableDomain)(Soduku *self, int variable);
  void (*showRemovals)(Soduku *self);
};

Soduku *initBoard(Soduku **self, char *initialPositions) {
  *self = malloc(sizeof(Soduku));
  checkNULL((void *)(*self));

  (*self)->csp = initCSP(&((*self)->csp));
  checkNULL((void *)(*self)->csp);
  support_pruning(&((*self)->csp));

  int variable;
  for (variable = 0; variable < NUM_SLOTS; variable++) {
    if (initialPositions[variable] == '.') {
      initialPositions[variable] = '0';
    }
    (*self)->csp->assignment[variable] = (int)initialPositions[variable] - 48;
  }

  return *self;
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

void destroySoduku(Soduku **self) {
  destroyCSP(&((*self)->csp));
  free((*self));
  *self = NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Incorrect number of arguments. Make sure there are no spaces in the board setup\n");
    return -1;
  }

  if (strlen(argv[1]) != NUM_SLOTS) {
    fprintf(stderr, "Incorrect argument size - should be a string of %d characters, with blanks/spaces represented by 0's or .'s\n", NUM_SLOTS);
    return -1;
  }

  Soduku *board = NULL;
  board = initBoard(&board, argv[1]);

  printf("Showing initial board setup...\n\n");
  display(board->csp);
  printf("\n\n Time to solve!\n\n");

  Queue *q = NULL;
  q = initQueue(&q);
  clock_t start, end;

  printf("Checking assignments...\n");
  int var;
  for (var = 0; var < NUM_SLOTS; var++) {
    printf("Variable %d: %d\n", var, board->csp->assignment[var]);
  }

  printf("Starting the solve with AC3 procedure...\n");
  start = clock();
  AC3(&q, &board->csp);
  end = clock();
  printf("AC3 solved the board in %f seconds.\n\n", difftime(end, start) / CLOCKS_PER_SEC);
  display(board->csp);
  printf("Freeing memory for the board...\n");
  destroySoduku(&board);
  printf("Board successully freed! \n\n Freeing memory for the queue...\n");
  destroyQueue(&q);
  printf("Queue successfully freed!\n");

  board = initBoard(&board, argv[1]);
  printf("Starting the solve with Backtracking procedure...\n");
  start = clock();
  backtracking_search(&board->csp);
  end = clock();
  printf("Backtracking solved the board in %f seconds.\n\n", difftime(end, start) / CLOCKS_PER_SEC);
  display(board->csp);
  printf("Freeing memory for the board...\n");
  destroySoduku(&board);
  printf("Board successfully freed!\n");

  return 0;
}

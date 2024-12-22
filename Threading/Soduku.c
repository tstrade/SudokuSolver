#include "Solver.h"
#include "CSP.h"
#include "datastructs.h"
#include "Solver.c"
#include "Soduku.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ASCII_9 57
#define ASCII_0 48
#define NUM_THREADS NUM_SLOTS * NUM_NEIGHBORS

pthread_t threads[NUM_THREADS];

CSP *initBoard(CSP *board, char *initialPositions) {
  board = initCSP(board);

  int slot;
  for (slot = 0; slot < NUM_SLOTS; slot++) {
    if (initialPositions[slot] > ASCII_0 && initialPositions[slot] <= ASCII_9) {
      board->assignment[slot] = (int)initialPositions[slot] - 48;
    } else {
      board->assignment[slot] = 0;
      board->numVars++;
    }
  }

  display(board);

  board->variables = malloc(board->numVars * sizeof(int));
  checkNULL((void *)board->variables);

  // Track all of the current variables on the board
  int i, var = 0;
  for (slot = 0; slot < NUM_SLOTS; slot++) {
    if (board->assignment[slot] == 0) {
      board->variables[var] = slot;
      var++;
    }
  }

  // Initialize the domains of all the slots
  // Variables will start with all possible values in their domain
  support_pruning(board);

  for (slot = 0; slot < NUM_SLOTS; slot++) {
    if (board->assignment[slot] == 0) { continue; }
    for (i = 0; i < NUM_VALUES; i++) {
      if ((i + 1) == board->assignment[slot]) { continue; }
      board->curr_domains[slot][i] = 0;
    }
  }

  // Remove values from variable's domain if a neighbor has already
  // been assigned that value
  int variable, n, neighbor;
  for (var = 0; var < board->numVars; var++) {
    variable = board->variables[var];
    for (n = 0; n < NUM_NEIGHBORS; n++) {
      neighbor = board->neighbors[variable][n];
      if (board->assignment[neighbor] != 0) {
	prune(board, variable, board->assignment[neighbor]);
      }
    }
  }

  // Update assignments in case initial setup resulted in obvious assignments
  // aka, domain reduced to just one value
  infer_assignment(board);

  return board;
}

void showVars(CSP *board) {
  int variable;
  printf("Variables: ");
  for (variable = 0; variable < board->numVars; variable++) {
    printf("%d  ", board->variables[variable]);
  }
  printf("\n\n");
}

void showCurrentDomains(CSP *board) {
  int slot;
  printf("Current domains are: \n");
  for (slot = 0; slot < NUM_SLOTS; slot++) {
    showVariableDomain(board, slot);
  }
}

void showVariableDomain(CSP *board, int variable) {
  int values;
  printf("Slot %d's current domain: ", variable);
  for (values = 0; values < NUM_VALUES; values++) {
    if (board->curr_domains[variable][values] != 0) {
      printf("%d  ", board->curr_domains[variable][values]);
    }
  }
  printf("\n");
}

void showRemovals(CSP *board) {
  int variable, values;
  printf("Current removed values for variable: ");
  for (variable = 0; variable < board->numVars; variable++) {
    printf("%d: ", board->variables[variable]);
    for (values = 0; values < NUM_VALUES; values++) {
      if (board->removals[variable][values] != 0) {
        printf("%d  ", board->removals[variable][values]);
      }
    }
    printf("\n\n");
  }
}

void clearBoard(CSP *board, Queue *q);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "\tIncorrect number of arguments. Make sure there are no spaces in the board setup\n");
    return -1;
  }

  if (strlen(argv[1]) != NUM_SLOTS) {
    fprintf(stderr, "\tIncorrect argument size - should be a string of %d characters, with blanks/spaces represented by 0's or .'s\n\n", NUM_SLOTS);
    return -1;
  }



  CSP *board = NULL;
  printf("  Showing initial board setup...\n");
  board = initBoard(board, argv[1]);


  Queue *q = NULL;
  q = initQueue(q);
  clock_t start, end;

  // initialize protections
  protect *knight = initProtect();
  // initialize args for AC3
  AC3_args *args = initAC3Args(board, q, 0, knight);
  // initialize threads
  long t;
  void *status;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  printf("\tTime to solve!\n\n");
  printf("  Starting the solve with AC3 procedure with %d threads...\n", NUM_THREADS);
  start = clock();

  for (t = 0; t < NUM_THREADS; t++) {
    args->slot = (int)t;
    pthread_create(&threads[t], &attr, AC3, (void *)args);
  }

  pthread_attr_destroy(&attr);

  for (t = 0; t < NUM_THREADS; t++) {
    pthread_join(threads[t], &status);
  }

  end = clock();

  printf("\tAC3 solved the board in %f seconds.\n\n", difftime(end, start) / CLOCKS_PER_SEC);
  display(board);

  printf("  Freeing memory for the board...\n");
  destroyAC3Args(args);
  printf("\tBoard successfully freed!\n");

  pthread_exit(NULL);

  return 0;
}

#include "Solver.h"
#include "CSP.h"
#include "datastructs.h"
#include "Solver.c"
#include "Soduku.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


CSP *initBoard(CSP *board, char *initialPositions) {
  board = initCSP(board);

  int variable, initAssignmentCount = 0;
  for (variable = 0; variable < NUM_SLOTS; variable++) {
    if (initialPositions[variable] == '.') {
      initialPositions[variable] = '0';
      initAssignmentCount++;
    }
    board->assignment[variable] = (int)initialPositions[variable] - 48;
  }

  board->numVars = initAssignmentCount;
  printf("This board has %d variables.\n\n", board->numVars);

  board->variables = malloc(board->numVars * sizeof(int));
  checkNULL((void *)board->variables);

  // Track all of the current variables on the board
  int v = 0;
  for (variable = 0; variable < NUM_SLOTS; variable++) {
    if (board->assignment[variable] == 0) {
      board->variables[v] = variable;
      v++;
    }
  }

  infer_assignment(board);

  showVars(board);
  showCurrentDomains(board);
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
  int variable;
  printf("Current variable domains are: \n");
  for (variable = 0; variable < board->numVars; variable++) {
    showVariableDomain(board, board->variables[variable]);
  }
}

void showVariableDomain(CSP *board, int variable) {
  int values;
  printf("Variable %d current domains: ", variable);
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
    fprintf(stderr, "Incorrect number of arguments. Make sure there are no spaces in the board setup\n");
    return -1;
  }

  if (strlen(argv[1]) != NUM_SLOTS) {
    fprintf(stderr, "Incorrect argument size - should be a string of %d characters, with blanks/spaces represented by 0's or .'s\n", NUM_SLOTS);
    return -1;
  }

  CSP *board = NULL;
  board = initBoard(board, argv[1]);

  printf("Showing initial board setup...\n\n");
  display(board);
  printf("\n\n Time to solve!\n\n");


  Queue *q = NULL;
  q = initQueue(q);
  clock_t start, end;

  /*
  printf("Checking neighbors...\n");
  int var, val;
  for (var = 0; var < NUM_SLOTS; var++) {
    printf("Neighbors of variable %d:", var);
    for (val = 0; val < NUM_NEIGHBORS; val++) {
      printf("  %d", board->csp->neighbors[var][val]);
    }
    printf("\n");
  }
  */





  printf("Starting the solve with AC3 procedure...\n");
  start = clock();
  int ac3Success = AC3(q, board);
  end = clock();
  if (ac3Success) {
    printf("AC3 solved the board in %f seconds.\n\n", difftime(end, start) / CLOCKS_PER_SEC);
  } else {
    printf("AC3 failed! Continue debugging :)\n\n");
  }
  display(board);
  printf("Freeing memory for the board...\n");
  //destroySoduku(board);
  //printf("Board successully freed! \n\n Freeing memory for the queue...\n");
  destroyQueue(q);
  printf("Queue successfully freed!\n");

  /*
  board = initBoard(board, argv[1]);
  printf("Starting the solve with Backtracking procedure...\n");
  start = clock();
  backtracking_search(board);
  end = clock();
  printf("Backtracking solved the board in %f seconds.\n\n", difftime(end, start) / CLOCKS_PER_SEC);
  display(board);
  printf("Freeing memory for the board...\n");
  destroySoduku(board);
  printf("Board successfully freed!\n");
  */

  return 0;
}

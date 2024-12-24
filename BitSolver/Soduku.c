#include "ABitsC3.h"
#include "CSP_Bits.h"
#include "qbit.h"
#include "ABitsC3.c"
#include "Soduku.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ASCII_9 57
#define ASCII_0 48

CSP *initBoard(char *initialPositions) {
  CSP *board = initCSP();

  ushort slot, variableCount = 0;
  for (slot = 0; slot < NUM_SLOTS; slot++) {
    if (initialPositions[slot] > ASCII_0 && initialPositions[slot] <= ASCII_9) {
      board->assignments[slot] = (ushort)initialPositions[slot] - 48;
    } else {
      board->assignments[slot] = 0;
      board->variables[++variableCount] = slot;
    }
  }

  board->variables = realloc(board->variables, (variableCount + 1) * sizeof(ushort));
  display(board);

  for (slot = 0; slot < NUM_SLOTS; slot++) {
    if (board->assignments[slot] == 0) { continue; }
    for (ushort value = 0; value < NUM_VALUES; value++) {
      if (board->assignments[slot] == (value + 1)) { continue; }
      board->domains[slot] &= (~(1 << value));
    }
  }

  // Remove values from variable's domain if a neighbor has already
  // been assigned that value
  ushort variable, n, neighbor;
  for (ushort var = 0; var < variableCount; var++) {
    variable = board->variables[var];

    for (n = 0; n < NUM_NEIGHBORS; n++) {
      neighbor = board->neighbors[variable][n];
      if (board->assignments[neighbor]) { prune(board, variable, 1 << n); }
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
  for (variable = 1; variable <= board->variables[0]; variable++) {
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
  ushort values = 0x0100;
  printf("Slot %d's current domain: ", variable);
  while (values) {
    if ((board->domains[variable] & values) != 0) {
      printf("%d  ", (int)logb(board->domains[variable] & values));
    }
    values >>= 1;
  }
  printf("\n");
}

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
  board = initBoard(argv[1]);
  printf("\tTime to solve!\n\n");

  clock_t start, end;
  ulong *qbit = NULL;

  printf("  Starting the solve with AC3 procedure...\n");
  start = clock();
  int ac3Success = ABitsC3(qbit, board);
  end = clock();

  if (ac3Success) {
    printf("\tAC3 solved the board in %f seconds.\n\n", difftime(end, start) / CLOCKS_PER_SEC);
  } else {
    printf("\tAC3 failed! Continue debugging :)\n\n");
  }
  display(board);

  printf("  Freeing memory for the board...\n");
  destroyCSP(board);
  printf("\tBoard successully freed! \n\n");

  for (int i = 0; i < 3; i++) {
    sleep(1);
    printf("\n");
  }

  return 0;
}

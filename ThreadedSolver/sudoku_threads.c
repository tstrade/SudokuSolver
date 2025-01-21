#include "sudoku_threads.h"
#include "AC3/AC3.h"
#include "CSP/CSP.h"
#include "Queue/queue.h"



pthread_t parent_thread, finish_thread;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "\tIncorrect number of arguments. Make sure there are no spaces in the board setup\n");
    return (int)FAILURE;
  }

  if (strlen(argv[1]) != NUM_SLOTS) {
    fprintf(stderr, "\tIncorrect argument size - should be a string of %d characters, with blanks/spaces represented by 0's or .'s\n\n", NUM_SLOTS);
    return (int)FAILURE;
  }

  if (qinit() == FAILURE) {
    fprintf(stderr, "\tFailed to initialize queue.\n");
    return (int)FAILURE;
  }

  if (ninit() == FAILURE) {
    fprintf(stderr, "\tFailed to initialize list of neighbors\n");
    return (int)FAILURE;
  }

  printf("  Showing initial board setup...\n");
  if (binit(argv[1]) == FAILURE) {
    fprintf(stderr, "\tFailed to initialize Sudoku Board\n");
    return (int)FAILURE;
  }


  //void *status;
  // clock_t start, end;


  return (int)SUCCESS;
}

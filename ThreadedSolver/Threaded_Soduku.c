#include "AC3/AC3_Split.h"
#include "CSP/CSP.h"
#include "Queue/queue.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ASCII_9 57
#define ASCII_0 48

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

  // initialize args for AC3
  AC3 *args = NULL;
  args = initAC3(board, q, &parent_thread, &finish_thread);

  //void *status;
  clock_t start, end;

  HOA *inspector;
  Revisors *worker;

  printf("\tTime to solve!\n\n");
  printf("  Starting the AC3 procedure...\n");
  start = clock();

  pthread_create(&(args->parent_thread), NULL, fetchQueue, args);
  pthread_create(&(args->finish_thread), NULL, editQueue, args);

  for (int slot = 0; slot < NUM_SLOTS; slot++) {
    inspector = args->inspectors[slot]; inspector->value = slot;
    pthread_create(&(inspector->thread), NULL, inspect, (void *)inspector);
    pthread_mutex_unlock(&(args->qLock));
  }

  for (int value = 0; value < NUM_VALUES; value++) {
    worker = args->workers[value]; worker->value = value;
    pthread_create(&(worker->thread), NULL, revise, worker);
  }

  printf("    Waiting for threads to finish...\n");
  pthread_join(args->finish_thread, NULL);

  end = clock();
  infer_assignment(board);

  printf("\tAC3 solved the board in %f seconds.\n\n", difftime(end, start) / CLOCKS_PER_SEC);
  display(board);

  printf("  Freeing memory for the board...\n");
  destroyAC3(args);
  printf("\tBoard successfully freed!\n");

  pthread_exit(NULL);

  return 0;
}

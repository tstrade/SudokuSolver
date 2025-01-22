#include "sudoku_threads.h"
#include "Queue/queue.h"
#include "CSP/CSP.h"
#include "AC3/AC3.h"

pthread_t r_threads[NUM_VALUES];
pthread_t v_threads[NUM_NEIGHBORS];
clock_t start;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "\tIncorrect number of arguments. Make sure there are no spaces in the board setup\n");
    return (int)FAILURE;
  }

  if (strlen(argv[1]) != NUM_SLOTS) {
    fprintf(stderr, "\tIncorrect argument size - should be a string of %d characters, with blanks/spaces represented by 0's or .'s\n\n", NUM_SLOTS);
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

  if (ac3init() == FAILURE) {
    fprintf(stderr, "\tFailed to initialize global data protections\n");
    return (int)FAILURE;
  }

  start = clock();

  for (uint8_t i = 0; i < NUM_VALUES; i++)
    pthread_create(&r_threads[i], NULL, revise, (void *)(&rtab[i]));

  for (uint8_t i = 0; i < NUM_NEIGHBORS; i++)
    pthread_create(&v_threads[i], NULL, verify, (void *)(&vtab[i]));

  printf("Ready to solve the %dx%d Sudoku board!\n", NUM_VALUES, NUM_VALUES);
  qcontrol(NULL);

  for (uint8_t i = 0; i < NUM_VALUES; i++)
    pthread_join(r_threads[i], NULL);

  for (uint8_t i = 0; i < NUM_NEIGHBORS; i++)
    pthread_join(v_threads[i], NULL);

  pthread_exit(NULL);
  return (int)SUCCESS;
}

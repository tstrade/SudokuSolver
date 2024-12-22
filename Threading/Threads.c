#include "Threads.h"
#include "CSP.c"
#include "datastructs.c"

#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sched.h>

int slot;

// AC3() will be the payload function for the threads, which takes (void *) as its only arg
struct AC3_args {
  CSP *csp;
  Queue *q;
  int *slot;
  protector *knight;
};
// Pointers all threads to be referencing the same item

// Initialize args - slot will be used to form the queue
AC3_args *initAC3Args(CSP *csp, Queue *q, protector *knight) {
  AC3_args *args = malloc(sizeof(AC3_args));
  checkNULL((void *)args);

  args->csp = csp;
  args->q = q;
  args->slot = 0;

  args->knight = knight;

  return args;
}

void destroyAC3Args(AC3_args *args) {
  destroyCSP(args->csp);
  destroyQueue(args->q);
  destroyProtector(args->knight);

  free(args);
  args = NULL;
}

void checkProtections(int status) {
  if (status) {
    fprintf(stderr, "Initialzing semaphore failed!\n");
    exit(EXIT_FAILURE);
  }
  return;
}
// Need to protect critical sections from race conditions
//
struct protector {
  sem_t pruning;
  int busyPruning;
  sem_t readingDomains;
  int busyReading;

  pthread_attr_t attr;
  pthread_mutex_t queue;
};

protector *initProtector() {
  protector *knight = malloc(sizeof(protector));
  checkNULL((void *)knight);

  checkProtections(sem_init(&knight->pruning, 0, 1));
  knight->busyPruning = 1;

  checkProtections(sem_init(&knight->readingDomains, 0, NUM_SLOTS));
  knight->busyReading = NUM_SLOTS;

  checkProtections(pthread_attr_init(&knight->attr));
  checkProtections(pthread_attr_setdetachstate(&knight->attr, PTHREAD_CREATE_JOINABLE));
  checkProtections(pthread_attr_setschedpolicy(&knight->attr, SCHED_RR));

  checkProtections(pthread_mutex_init(&knight->queue, NULL));

  return knight;
}

void destroyProtector(protector *knight) {
  sem_destroy(&knight->pruning);
  sem_destroy(&knight->readingDomains);

  pthread_attr_destroy(&knight->attr);
  pthread_mutex_destroy(&knight->queue);

  free(knight);
  knight = NULL;
}

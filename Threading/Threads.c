#include "Threads.h"
#include "CSP.c"
#include "datastructs.c"

#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

int slot;

// AC3() will be the payload function for the threads, which takes (void *) as its only arg
struct AC3_args {
  CSP *csp;
  Queue *q;
  int slot;
  protector *knight;
};
// Pointers all threads to be referencing the same item

// Initialize args - slot will be used to form the queue
AC3_args *initAC3Args(CSP *csp, Queue *q, protector *knight) {
  AC3_args *args = malloc(sizeof(AC3_args));
  checkNULL((void *)args);

  args->csp = csp;
  args->q = q;

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

// Need to protect critical sections from race conditions
struct protector {
  sem_t pruning;
  int busyPruning;
  sem_t readingDomains;
  int busyReading;

  pthread_attr_t attr;
  pthread_cond_t cond;
  pthread_mutex_t queue;
  sem_t editingQ;
  int busyEditing;
  sem_t searchingQ;
  int busySearching;
};

protector *initProtector() {
  protector *knight = malloc(sizeof(protector));
  checkNULL((void *)knight);

  sem_init(&knight->pruning, 0, 1);
  knight->busyPruning = 1;

  sem_init(&knight->readingDomains, 0, NUM_SLOTS);
  knight->busyReading = NUM_SLOTS;

  pthread_attr_init(&knight->attr);
  pthread_attr_setdetachstate(&knight->attr, PTHREAD_CREATE_JOINABLE);

  pthread_cond_init(&knight->cond, NULL);

  pthread_mutex_init(&knight->queue, NULL);

  sem_init(&knight->editingQ, 0, 1);
  knight->busyEditing = 1;

  sem_init(&knight->searchingQ, 0, NUM_SLOTS / 3);
  knight->busySearching = NUM_SLOTS / 3;

  return knight;
}

void destroyProtector(protector *knight) {
  sem_destroy(&knight->pruning);
  sem_destroy(&knight->readingDomains);

  pthread_attr_destroy(&knight->attr);
  pthread_cond_destroy(&knight->cond);
  pthread_mutex_destroy(&knight->queue);
  sem_destroy(&knight->editingQ);
  sem_destroy(&knight->searchingQ);

  free(knight);
  knight = NULL;
}

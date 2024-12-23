#include "Threads.h"
#include "CSP.c"
#include "datastructs.c"

#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int slot;

// AC3() will be the payload function for the threads, which takes (void *) as its only arg
struct AC3_args {
  CSP *csp;
  int slot;
  protector *knight;
};
// Pointers all threads to be referencing the same item

// Initialize args - slot will be used to form the queue
AC3_args *initAC3Args(CSP *csp, protector *knight) {
  AC3_args *args = malloc(sizeof(AC3_args));
  checkNULL((void *)args);

  args->csp = csp;
  args->knight = knight;

  return args;
}

void destroyAC3Args(AC3_args *args) {
  destroyCSP(args->csp);
  destroyProtector(args->knight);

  free(args);
  args = NULL;
}

// Need to protect critical sections from race conditions
struct protector {
  pthread_rwlock_t rwDomains;

  pthread_attr_t attr;
  pthread_mutex_t queue;
  pthread_mutex_t assigningSlot;
  int goalTest;

  pthread_rwlock_t rwQueue;
};

protector *initProtector() {
  protector *knight = malloc(sizeof(protector));
  checkNULL((void *)knight);

  pthread_rwlock_init(&knight->rwDomains, NULL);

  pthread_attr_init(&knight->attr);
  pthread_attr_setdetachstate(&knight->attr, PTHREAD_CREATE_JOINABLE);

  pthread_mutex_init(&knight->queue, NULL);
  pthread_mutex_init(&knight->assigningSlot, NULL);
  knight->goalTest = 0;

  pthread_rwlock_init(&knight->rwQueue, NULL);

  return knight;
}

void destroyProtector(protector *knight) {
  pthread_rwlock_destroy(&knight->rwDomains);
  pthread_attr_destroy(&knight->attr);
  pthread_mutex_destroy(&knight->queue);
  pthread_mutex_destroy(&knight->assigningSlot);
  pthread_rwlock_destroy(&knight->rwQueue);

  free(knight);
  knight = NULL;
}

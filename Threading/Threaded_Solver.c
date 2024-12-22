#include "Threaded_Solver.h"
#include "Threads.c"

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *AC3(void *args) {
  CSP *csp = ((AC3_args *)(args))->csp;
  Queue *q = ((AC3_args *)(args))->q;
  int *slot = ((AC3_args *)(args))->slot;
  protector *knight = ((AC3_args *)(args))->knight;

  while (*slot < NUM_SLOTS) {
    pthread_mutex_lock(&knight->queue);
    get_queue(csp, q, *slot);
    (*slot)++;
    pthread_mutex_unlock(&knight->queue);
  }

  int X[2] = { 0, 0 };
  int domain, qIndex, isInQueue, nIndex, isNeighbor;
  int Xi, Xj;

  // Don't read the size while it is changing
  pthread_mutex_lock(&knight->queue);
  while (q->currSize != 0) {
    // Locking at the end of the while loop keeps the queue and it's size protected
    dequeue(q, X);
    pthread_mutex_unlock(&knight->queue);

    Xi = X[0], Xj = X[1];

    // Check for constraint violations between first pair of neighbors
    if (revise(csp, Xi, Xj, knight) == 1) {

      sem_wait(&knight->pruning); // If pruning, wait until done
      if (count(csp->curr_domains[X[0]]) == 0) { fprintf(stderr, "AC3 Failed!\n"); exit(EXIT_FAILURE); }
      sem_post(&knight->pruning); // If only reading, let other threads pass

      for (domain = 0; domain < NUM_SLOTS; domain++) {
	// Don't enqueue a tuple consisting of the same variable ((0,0), (1,1), etc.)
	if (domain == X[0]) { continue; }

	// Don't enqueue a tuple consisting of non-neighboring variables (constraint doesn't apply)
	nIndex = 0, isNeighbor = 0;
        while (nIndex != NUM_NEIGHBORS && isNeighbor != 1) {
	  if (X[0] == csp->neighbors[domain][nIndex]) {
	    isNeighbor = 1;
	  }
	  nIndex++;
	}
	if (isNeighbor == 0) { continue; }

	// Don't enqueue the tuple exists in the queue already
	pthread_mutex_lock(&knight->queue); // If writing/reading queue, do not read/write queue
	qIndex = q->head, isInQueue = 0;
	while (qIndex != q->tail && isInQueue != 1) {
	  if (domain == q->tuples[qIndex][0] && X[0] == q->tuples[qIndex][1]) {
	    isInQueue = 1;
	  }
	  qIndex = (qIndex + 1) % q->maxSize;
	}

	// Don't unlock the mutex until tuple has either been queued
	// or found to be already in the queue
	if (isInQueue == 1) {
	  pthread_mutex_unlock(&knight->queue);
	  continue;
	} else {
	  enqueue(q, domain, X[0]);
	  pthread_mutex_unlock(&knight->queue);
	}

      } // End for loop
    } // End revise
    pthread_mutex_lock(&knight->queue);
  } // End while
  pthread_mutex_unlock(&knight->queue);

  pthread_exit((void *) 0);
}

int revise(CSP *csp, int Xi, int Xj, protector *knight) {
  int revised = 0; // false

  // Want to see which values in Xi's current domain satisfy the rules of Soduku
  int XiValue, XjValue, valXi, valXj, satisfied;

  for (valXi = 0; valXi < NUM_VALUES; valXi++) {
    while (knight->busyPruning == 0) { sem_getvalue(&knight->pruning, &knight->busyPruning); }
    sem_wait(&knight->readingDomains);
    XiValue = csp->curr_domains[Xi][valXi];
    sem_post(&knight->readingDomains);
    // If val is not in Xi's current domain, there is no conflict
    if (XiValue == 0) { continue; }

    satisfied = 0;
    for (valXj = 0; valXj < NUM_VALUES; valXj++) {

      while (knight->busyPruning == 0) { sem_getvalue(&knight->pruning, &knight->busyPruning); }
      // Allow NUM_SLOTS threads to read domains at a time
      // will also end up blocking prune() until no one is reading
      sem_wait(&knight->readingDomains);
      XjValue = csp->curr_domains[Xj][valXj];
      sem_post(&knight->readingDomains);

      // If val is not in Xj's current domain, there is no conflict
      if (XjValue == 0) { continue; }

      // Since Xi and Xj are neighbors (as per the enqueue process in AC3),
      // we check if the constraint is satisfied between their current domains
      satisfied += csp->constraint(Xi, XiValue, Xj, XjValue);

    } // End Xj domain loop

    // If the constraint is violated, remove the value from Xi's domain
    if (satisfied == 0) {
      while (knight->busyReading < NUM_SLOTS) { sem_getvalue(&knight->readingDomains, &knight->busyReading); }
      // Only one thread should be able to prune at a time
      sem_wait(&knight->pruning);
      prune(csp, Xi, XiValue);
      sem_post(&knight->pruning);


      revised = 1;
    }
  } // End Xi domain loop

  return revised;
}

void get_queue(CSP *csp, Queue *q, int variable) {
  // We want to find each variable-variable pair that is in the same row/column/box,
  // which will help us check our possible solution against our constraint
  int nIndex;
  for (nIndex = 0; nIndex < NUM_NEIGHBORS; nIndex++) {
    enqueue(q, csp->neighbors[variable][nIndex], variable);
  }
}

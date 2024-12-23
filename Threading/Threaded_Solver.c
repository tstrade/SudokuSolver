#include "Threaded_Solver.h"
#include "Threads.c"

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void *AC3(void *args) {
  CSP *csp = ((AC3_args *)(args))->csp;
  Queue *q = ((AC3_args *)(args))->q;
  int slot = ((AC3_args *)(args))->slot;
  protector *knight = ((AC3_args *)(args))->knight;

  get_queue(csp, q, slot);

  int X[2] = { 0, 0 };
  int domain, qIndex, isInQueue, nIndex, isNeighbor;
  int Xi, Xj;
  while (q->currSize != NUM_SLOTS * NUM_NEIGHBORS);

  while (1) {
    pthread_mutex_lock(&knight->queue);
    if (isEmpty(q)) { pthread_mutex_unlock(&knight->queue); break; }
    dequeue(q, X);
    qIndex = q->head;
    pthread_mutex_unlock(&knight->queue);

    Xi = X[0], Xj = X[1];

    // Check for constraint violations between first pair of neighbors
    if (revise(csp, Xi, Xj, knight) == 1) {

      // Hypothetically not a CS - thread will simply catch if AC3 has failed sooner than normal
      if (count(csp->curr_domains[X[0]]) == 0) { fprintf(stderr, "AC3 Failed!\n"); exit(EXIT_FAILURE); }

      for (domain = 0; domain < NUM_SLOTS; domain++) {
	// Don't enqueue a tuple consisting of the same variable ((0,0), (1,1), etc.)
	if (domain == X[0]) { continue; }

	// Don't enqueue a tuple consisting of non-neighboring variables (constraint doesn't apply)
	nIndex = 0, isNeighbor = 0;
        while (nIndex != NUM_NEIGHBORS && isNeighbor != 1) {
	  if (X[0] == csp->neighbors[domain][nIndex]) { isNeighbor = 1; }
	  nIndex++;
	}

	if (isNeighbor == 0) { continue; }

        isInQueue = 0; // Don't enqueue the tuple if it exists in the queue already

	sem_wait(&knight->searchingQ);
	while (knight->busyEditing == 0) { sem_getvalue(&knight->editingQ, &knight->busyEditing); }

	while (qIndex != q->tail) {
	  if (domain == q->tuples[qIndex][0] && X[0] == q->tuples[qIndex][1]) { isInQueue = 1; break; }
	  else { qIndex = (qIndex + 1) % q->maxSize; }
	}
	sem_post(&knight->searchingQ);

	// Don't unlock the mutex until tuple has either been queued or found to be already in the queue
	if (isInQueue == 1) { pthread_mutex_unlock(&knight->queue); continue; }
	else {
	  while (knight->busySearching < NUM_SLOTS / 3) { sem_getvalue(&knight->searchingQ, &knight->busySearching); }

	  sem_wait(&knight->editingQ);
	  pthread_mutex_lock(&knight->queue);
	  enqueue(q, domain, X[0]);
	  sem_post(&knight->editingQ);
	  pthread_mutex_unlock(&knight->queue);
	}

      } // End for loop
    } // End revise
  } // End while

  pthread_exit((void *) 0);
}

int revise(CSP *csp, int Xi, int Xj, protector *knight) {
  int revised = 0; // false

  // Want to see which values in Xi's current domain satisfy the rules of Soduku
  int XiValue, XjValue, valXi, valXj, satisfied;

  for (valXi = 0; valXi < NUM_VALUES; valXi++) {
    // Polling for whether or not prune() is being executed
    sem_wait(&knight->readingDomains);
    while (knight->busyPruning == 0) { sem_getvalue(&knight->pruning, &knight->busyPruning); }
    XiValue = csp->curr_domains[Xi][valXi];
    sem_post(&knight->readingDomains);

    // If val is not in Xi's current domain, there is no conflict
    if (XiValue == 0) { continue; }

    satisfied = 0;
    for (valXj = 0; valXj < NUM_VALUES; valXj++) {

      sem_wait(&knight->readingDomains);
      while (knight->busyPruning == 0) { sem_getvalue(&knight->pruning, &knight->busyPruning); }
      XjValue = csp->curr_domains[Xj][valXj];
      sem_post(&knight->readingDomains);

      // If val is not in Xj's current domain, there is no conflict
      if (XjValue == 0) { continue; }

      // Check if the constraint is satisfied between current domains
      satisfied += csp->constraint(Xi, XiValue, Xj, XjValue);

    } // End Xj domain loop

    // If the constraint is violated, remove the value from Xi's domain
    if (satisfied == 0) {
      // Polling for whether or not there are any threads reading the domains
      sem_wait(&knight->pruning);
      while (knight->busyReading < NUM_SLOTS / 3) { sem_getvalue(&knight->readingDomains, &knight->busyReading); }
      prune(csp, Xi, XiValue);
      sem_post(&knight->pruning);

      revised = 1;
    }
  } // End Xi domain loop

  return revised;
}

void get_queue(CSP *csp, Queue *q, int variable) {
  // We want to find each (VarA,VarB) that is in the same row/column/box,
  // which will help us check our possible solution against our constraint
  int nIndex;
  for (nIndex = 0; nIndex < NUM_NEIGHBORS; nIndex++) {
    enqueue(q, csp->neighbors[variable][nIndex], variable);
  }
}

#include "Solver.h"
#include "CSP.c"
#include "datastructs.c"

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct AC3_args {
  CSP *csp;
  Queue *q;
  int slot;
  protect *knight;
};

AC3_args *initAC3Args(CSP *csp, Queue *q, int slot, protect *knight) {
  AC3_args *args = malloc(sizeof(AC3_args));
  checkNULL((void *)args);

  args->csp = csp;
  args->q = q;
  args->slot = slot;

  args->knight = knight;

  return args;
}

struct protect {
  volatile int pruning;
  volatile int readingDomains;

  pthread_mutex_t queue;
  volatile int readingSize;
  volatile int queueReceived;
};

protect *initProtect() {
  protect *knight = malloc(sizeof(protect));
  checkNULL((void *)knight);

  knight->pruning = 0;
  knight->readingDomains = 0;
  knight->readingSize = 0;
  pthread_mutex_init(&knight->queue, NULL);

  knight->queueReceived = 0;

  return knight;
}

void *AC3(void *args) {
  CSP *csp = ((AC3_args *)(args))->csp;
  Queue *q = ((AC3_args *)(args))->q;
  int slot = ((AC3_args *)(args))->slot;
  protect *knight = ((AC3_args *)(args))->knight;

  pthread_mutex_lock(&knight->queue);
  get_queue(csp, q, slot);
  pthread_mutex_unlock(&knight->queue);

  while(q->currSize < NUM_SLOTS * NUM_NEIGHBORS || knight->queueReceived);
  knight->queueReceived = 1;

  int *X = calloc(2, sizeof(int));
  int domain, qIndex, isInQueue, nIndex, isNeighbor;
  while (q->currSize != 0) {
    // Lock inside queue
    pthread_mutex_lock(&knight->queue);
    dequeue(q, X);
    pthread_mutex_unlock(&knight->queue);

    // Check for constraint violations between first pair of neighbors
    if (revise(csp, X[0], X[1], knight) == 1) {

      while (knight->pruning);
      knight->readingDomains++;
      if (count(csp->curr_domains[X[0]]) == 0) {
	fprintf(stderr, "AC3 Failed!\n"); exit(EXIT_FAILURE);
      }
      knight->readingDomains--;

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
	pthread_mutex_lock(&knight->queue);
	qIndex = q->head, isInQueue = 0;
	while (qIndex != q->tail && isInQueue != 1) {
	  if (domain == q->tuples[qIndex][0] && X[0] == q->tuples[qIndex][1]) {
	    isInQueue = 1;
	  }
	  qIndex = (qIndex + 1) % q->maxSize;
	}

	if (isInQueue == 1) {
	  pthread_mutex_unlock(&knight->queue);
	  continue;
	} else {
	  enqueue(q, domain, X[0]);
	  pthread_mutex_unlock(&knight->queue);
	}

      } // End for loop
    } // End revise
  } // End while

  free(X);
  X = NULL;

  pthread_exit((void *) 0);
}

int revise(CSP *csp, int Xi, int Xj, protect *knight) {
  int revised = 0; // false

  // Want to see which values in Xi's current domain satisfy the rules of Soduku
  int XiValue, XjValue, valXi, valXj, satisfied;

  for (valXi = 0; valXi < NUM_VALUES; valXi++) {
    // If val is not in Xi's current domain, there is no conflict

    while (knight->pruning);
    knight->readingDomains++;
    XiValue = csp->curr_domains[Xi][valXi];
    knight->readingDomains--;

    if (XiValue == 0) { continue; }

    satisfied = 0;
    for (valXj = 0; valXj < NUM_VALUES; valXj++) {
      while (knight->pruning);
      knight->readingDomains++;
      XjValue = csp->curr_domains[Xj][valXj];
      knight->readingDomains--;

      // If val is not in Xj's current domain, there is no conflict
      if (XjValue == 0) { continue; }

      // Since Xi and Xj are neighbors (as per the enqueue process in AC3),
      // we check if the constraint is satisfied between their current domains
      satisfied += csp->constraint(Xi, XiValue, Xj, XjValue);

    } // End Xj domain loop

    // If the constraint is violated, remove the value from Xi's domain
    if (satisfied == 0) {
      while (knight->readingDomains > 0);
      knight->pruning = 1;
      prune(csp, Xi, XiValue);
      knight->pruning = 0;

      // Unlock removals and domains
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

void destroyAC3Args(AC3_args *args) {
  destroyCSP(args->csp);
  destroyQueue(args->q);
  destroyProtect(args->knight);

  free(args);
  args = NULL;
}

void destroyProtect(protect *knight) {
  pthread_mutex_destroy(&knight->queue);

  free(knight);
  knight = NULL;
}

#include "Threaded_Solver.h"
#include "Threads.c"

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void *AC3(void *args) {
  CSP *csp = ((AC3_args *)args)->csp;
  protector *knight = ((AC3_args *)args)->knight;

  Queue *q = NULL;
  q = initQueue(q);

  pthread_mutex_lock(&knight->assigningSlot);
  int slot = ((AC3_args *)args)->slot++;
  pthread_mutex_unlock(&knight->assigningSlot);

  get_queue(csp, q, slot);

  int Xi, Xj, X[2] = { 0, 0 };
  int domain, qIndex, isInQueue, nIndex, isNeighbor;

  while (1) {
    while (isEmpty(q)) { if (knight->goalTest) { break; } }

    pthread_mutex_lock(&knight->queue);
    dequeue(q, X);
    qIndex = q->head; // Might want to try moving this
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

	// Acquire read lock to scan queue for tuple - queue writers will be blocked
	// Will block if queue writer is already waiting to acquire lock (bounded waiting)
        pthread_rwlock_rdlock(&knight->rwQueue);
	while (qIndex != q->tail) {
	  if (domain == q->tuples[qIndex][0] && X[0] == q->tuples[qIndex][1]) { isInQueue = 1; break; }
	  qIndex = (qIndex + 1) % q->maxSize;
	}
	// Release read lock to allow queue writers to acquire lock
        pthread_rwlock_unlock(&knight->rwQueue);

	// If the tuple is in the queue, skip to the next tuple option
	if (isInQueue == 1) { continue; }

	// Acquire queue lock to avoid clashing with currSize, head, and tail
	pthread_mutex_lock(&knight->queue);
	// Acquire write lock to add item to queue - queue readers/writers will be blocked
	// Will block if queue writer or queue reader already has lock
	pthread_rwlock_wrlock(&knight->rwQueue);
	enqueue(q, domain, X[0]);
	// Release write lock to allow queue readers/writers to acquire lock
	pthread_rwlock_unlock(&knight->rwQueue);
	// Release queue lock to allow other threads to enqueue/dequeue
	pthread_mutex_unlock(&knight->queue);

      } // End for loop
    } // End revise

    pthread_mutex_lock(&knight->assigningSlot);

    pthread_rwlock_rdlock(&knight->rwDomains);
    infer_assignment(csp);
    pthread_rwlock_unlock(&knight->rwDomains);

    knight->goalTest = goal_test(csp);
    pthread_mutex_unlock(&knight->assigningSlot);

  } // End while

  pthread_exit((void *) 0);
}

int revise(CSP *csp, int Xi, int Xj, protector *knight) {
  int revised = 0; // false

  // Want to see which values in Xi's current domain satisfy the rules of Soduku
  int XiValue, XjValue, valXi, valXj, satisfied;

  for (valXi = 0; valXi < NUM_VALUES; valXi++) {
    // Acquire read lock to retrieve domain value
    pthread_rwlock_rdlock(&knight->rwDomains);
    XiValue = csp->curr_domains[Xi][valXi];
    pthread_rwlock_unlock(&knight->rwDomains);
    // Release read lock to allow writers to acquire lock

    // If val is not in Xi's current domain, there is no conflict
    if (XiValue == 0) { continue; }

    satisfied = 0;
    for (valXj = 0; valXj < NUM_VALUES; valXj++) {
      // Acquire read lock to retrieve domain value
      pthread_rwlock_rdlock(&knight->rwDomains);
      XjValue = csp->curr_domains[Xj][valXj];
      pthread_rwlock_unlock(&knight->rwDomains);
      // Release read lock to allow writers to acquire lock

      // If val is not in Xj's current domain, there is no conflict
      if (XjValue == 0) { continue; }

      // Check if the constraint is satisfied between current domains
      satisfied += csp->constraint(Xi, XiValue, Xj, XjValue);

    } // End Xj domain loop

    // If the constraint is violated, remove the value from Xi's domain
    if (satisfied == 0) {

      // Acquire write lock to change domain value
      pthread_rwlock_wrlock(&knight->rwDomains);
      prune(csp, Xi, XiValue);
      pthread_rwlock_unlock(&knight->rwDomains);
      // Release lock to allow readers/writers to acquire lock

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

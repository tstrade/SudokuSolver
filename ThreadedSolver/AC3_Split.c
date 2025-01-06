#include "AC3_Split.h"
#include "AC3_structs.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


//#include "CSP.c"
//#include "queue.c"
#include "AC3_structs.c"

#ifndef NUM_SLOTS
#define NUM_SLOTS 81
#endif

#ifndef NUM_NEIGHBORS
#define NUM_NEIGHBORS 20
#endif

#ifndef NUM_VALUES
#define NUM_VALUES 9
#endif


// Single thread function
void *fetchQueue(void *arg) { // .....................................................................................................................Start fetching queue
  AC3 *fetched = (AC3 *)arg;

  for (int slot = 0; slot < NUM_SLOTS; slot++) {
    //pthread_mutex_lock(&(fetched->qLock)); // .......................Locking qLock (AC3)
    for (int n = 0; n < NUM_NEIGHBORS; n++) {
      enqueue(fetched->q, fetched->csp->neighbors[slot][n], slot);
    }
    //pthread_mutex_unlock(&(fetched->qLock)); // ...................Unlocking qLock (AC3)
  }

  pthread_cond_broadcast(&fetched->fetched);

  scanQueue(fetched);

  pthread_exit(NULL);

} // ...................................................................................................................................................End fetching queue




// Single thread function
void *editQueue(void *arg) { // .......................................................................................................................Start editing queue
  AC3 *fetched = (AC3 *)arg;

  pthread_mutex_lock(&(fetched->inspecting));
  pthread_cond_wait(&(fetched->fetched), &(fetched->inspecting));
  pthread_mutex_unlock(&(fetched->inspecting));

  while (fetched->q->currSize != 0) { // .....................................................................................Starting while loop

    pthread_mutex_lock(&(fetched->qLock)); // ......................................Locking qLock (AC3)

    dequeue(fetched->q, fetched->tuple); // Dequeue
    pthread_cond_broadcast(&(fetched->StartRevise)); // Signaling StartRevise (AC3)

    // Need to be signaled by revisors if the tuple has no conflicts
    // Then scanning and inspectors can start
    pthread_mutex_unlock(&(fetched->qLock)); // ..................................Unlocking qLock (AC3)

    pthread_mutex_lock(&(fetched->inspecting)); // ....................................................Locking inspecting (AC3)

    for (int i = 0; i < NUM_SLOTS; i++) {
      pthread_cond_wait((&(fetched->inspectors[i])->EndInspect), &(fetched->inspecting)); // ..........Waiting EndInspect (AC3)
    }

    pthread_mutex_unlock(&(fetched->inspecting)); // .................................................Unlocking inspecing (AC3)

  } // ........................................................................................................................Ending while loop

  fetched->completed = 1;
  printf("Edit queue is finished.\n");
  pthread_exit(NULL);

} // ...................................................................................................................................................End editing queue




// Single thread function
void *scanQueue(void *arg) { // ......................................................................................................................Start scaning queue
  AC3 *fetched = (AC3 *)arg;
  int i;

  while (!fetched->completed) { // ...........................................................................................Starting while loop

    pthread_mutex_lock(&(fetched->revising)); // ....................................................Locking revising (AC3)

    for (i = 0; i < NUM_VALUES; i++) {
      pthread_cond_wait((&(fetched->workers[i])->EndRevise), &(fetched->revising)); // Waiting EndRevise (Revisors[i], AC3)
    }

    if (count(fetched->csp->curr_domains[fetched->tuple[0]], NUM_VALUES) == 0) {
      fprintf(stderr, "  Found empty domain!\n"); exit(EXIT_FAILURE); }

    pthread_cond_broadcast(&(fetched->StartInspecting)); // Signaling StartInspecting (AC3)

    pthread_mutex_unlock(&(fetched->revising)); // ................................................Unlocking revising (AC3)

  } // ........................................................................................................................Ending while loop

  pthread_exit(NULL);

} // .................................................................................................................................................End scanning queue




// Multi thread function
void *revise(void *arg) { // .......................................................................................................................Start revising queue
  Revisors *worker = (Revisors *)arg;
  CSP *csp = worker->top_level->csp;
  int Xi, Xj, XiValue, XjValue, domainValXj;

  pthread_mutex_lock(&worker->WaitFetch);
  pthread_cond_wait(&worker->top_level->fetched, &worker->WaitFetch);
  pthread_mutex_unlock(&worker->WaitFetch);

  while (!worker->top_level->completed) { // ...............................................................................Starting while loop

    pthread_mutex_lock(&(worker->WaitRevise)); // .................................................Locking WaitRevise (Revisors)

    pthread_cond_wait(&(worker->top_level->StartRevise), &(worker->WaitRevise)); // ..............Waiting StartRevise (AC3, Revisors)

    worker->satisfied = 0;
    Xi = worker->top_level->tuple[0], Xj = worker->top_level->tuple[1];
    XiValue = csp->curr_domains[Xi][worker->value];

    if (XiValue == 0) { continue; }

    for (domainValXj = 0; domainValXj < NUM_VALUES; domainValXj++) {
      XjValue = csp->curr_domains[Xj][domainValXj];
      if (XjValue == 0) { continue; }

      worker->satisfied += csp->constraint(Xi, XiValue, Xj, XjValue);

    } // End Xj domain loop

    if (!worker->satisfied) { prune(csp, Xi, XiValue); worker->top_level->revised = 1; }

    pthread_cond_signal(&(worker->EndRevise)); // Signaling EndRevise (Revisors)
    pthread_mutex_unlock(&(worker->WaitRevise)); // .............................................Unlocking WaitRevise

  } // .......................................................................................................................Ending while loop

  pthread_exit(NULL);

} // ................................................................................................................................................End revising queue




// Multi thread function
void *inspect(void *arg) { // ...................................................................................................................Start inspecting queue
  HOA *inspector = (HOA *)arg;
  Tuple *head;

  pthread_mutex_lock(&inspector->WaitFetch);
  pthread_cond_wait(&inspector->top_level->fetched, &inspector->WaitFetch);
  pthread_mutex_unlock(&inspector->WaitFetch);

  while (!inspector->top_level->completed) { // ...........................................................................Starting while loop

    pthread_mutex_lock(&(inspector->WaitInspect)); // .............................................Locking WaitInspect (HOA)

    pthread_cond_wait(&(inspector->top_level->StartInspecting), &(inspector->WaitInspect)); // Waiting StartInspecting (AC3, HOA)

    if (inspector->value == inspector->top_level->tuple[1]) {
      goto not_valid;
    }

    for (int n = 0; n < NUM_NEIGHBORS; n++) {
      if (inspector->top_level->tuple[0] == inspector->top_level->csp->neighbors[inspector->value][n]) {
        goto is_neighbor;
      }
    }
    goto not_valid;

  is_neighbor:
    head = inspector->top_level->q->head;
    while (head) {
      if (head->Xi == inspector->value && head->Xj == inspector->top_level->tuple[0]) { goto not_valid; }
      head = head->next;
    }

    pthread_mutex_lock(&(inspector->top_level->qLock)); // ..................Locking qLock (AC3)
    enqueue(inspector->top_level->q, inspector->value, inspector->top_level->tuple[0]);
    pthread_mutex_unlock(&(inspector->top_level->qLock)); // ..............Unlocking qLock (AC3)

  not_valid:
    pthread_cond_signal(&(inspector->EndInspect)); // Signaling EndInspect (HOA)
    pthread_mutex_unlock(&(inspector->WaitInspect)); // .........................................Unlocking WaitInspect (HOA)

  } // .....................................................................................................................Ending while loop

  pthread_exit(NULL);

} // ............................................................................................................................................End inspecting queue

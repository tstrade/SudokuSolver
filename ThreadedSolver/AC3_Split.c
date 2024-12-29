#include "AC3_Split.h"
#include "AC3_structs.h"

#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>

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

void *fetchQueue(void *arg) {
  AC3 *fetched = (AC3 *)arg;
  int thread_dispatched = 0;
  HOA *inspector;

  for (int slot = 0; slot < NUM_SLOTS; slot++) {
    pthread_mutex_lock(&(fetched->qLock));
    for (int n = 0; n < NUM_NEIGHBORS; n++) {
      enqueue(fetched->q, fetched->csp->neighbors[slot][n], slot);
      if (!thread_dispatched) { pthread_create(&(fetched->finish_thread), NULL, editQueue, arg); thread_dispatched = 1; }
    }
    inspector = fetched->inspectors[slot]; inspector->value = slot;
    pthread_create(&(inspector->thread), NULL, inspect, (void *)inspector);
    pthread_mutex_unlock(&(fetched->qLock));
  }

  scanQueue(arg);

  pthread_exit(NULL);
}

void *editQueue(void *arg) {
  AC3 *fetched = (AC3 *)arg;
  Revisors *worker;

  for (int value = 0; value < NUM_VALUES; value++) {
    worker = fetched->workers[value]; worker->value = value;
    pthread_create(&(worker->thread), NULL, revise, worker);
  }


  while (fetched->q->currSize != 0) {
    pthread_mutex_lock(&(fetched->qLock)); // Lock queue from read/write

    dequeue(fetched->q, fetched->tuple); // dequeue
    pthread_cond_broadcast(&(fetched->StartRevise)); // let revisions begin
    pthread_mutex_unlock(&(fetched->qLock)); // Unlock queue to read/write


    pthread_mutex_lock(&(fetched->inspecting)); // Wait for other threads to evaluate conditions
    for (int i = 0; i < NUM_SLOTS; i++) {
      pthread_cond_wait((&(fetched->inspectors[i])->EndInspect), &(fetched->inspecting));
    }

    pthread_mutex_unlock(&(fetched->inspecting));
  }

  /*
  for (int i = 0; i < NUM_VALUES; i++) {
    pthread_cancel((fetched->workers[i])->thread);
  }

  for (int j = 0; j < NUM_SLOTS; j++) {
    pthread_cancel((fetched->inspectors[j])->thread);
  }

  pthread_cancel(fetched->parent_thread);
  */

  fetched->completed = 1;
  pthread_exit(NULL);
}

void *scanQueue(void *arg) {
  AC3 *fetched = (AC3 *)arg;
  int i;

  while (!fetched->completed) {
    pthread_mutex_lock(&(fetched->revising));

    for (i = 0; i < NUM_VALUES; i++) {
      pthread_cond_wait((&(fetched->workers[i])->EndRevise), &(fetched->revising));
    }

    if (count(fetched->csp->curr_domains[fetched->tuple[0]], NUM_VALUES) == 0) {
      fprintf(stderr, "  Found empty domain!\n"); exit(EXIT_FAILURE); }

    pthread_cond_broadcast(&(fetched->StartInspecting));

    pthread_mutex_unlock(&(fetched->revising));
  }

  pthread_exit(NULL);
}

void *revise(void *arg) {
  Revisors *worker = (Revisors *)arg;
  CSP *csp = worker->top_level->csp;
  int Xi, Xj, XiValue, XjValue, domainValXj;

  while (!worker->top_level->completed) {
    pthread_mutex_lock(&(worker->WaitRevise));
    pthread_cond_wait(&(worker->top_level->StartRevise), &(worker->WaitRevise));

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
    pthread_cond_broadcast(&(worker->EndRevise));
    pthread_mutex_unlock(&(worker->WaitRevise));
  }

  pthread_exit(NULL);
}

void *inspect(void *arg) {
  HOA *inspector = (HOA *)arg;
  Tuple *head;

  while (!inspector->top_level->completed) {
    pthread_mutex_lock(&(inspector->WaitInspect));
    pthread_cond_wait(&(inspector->top_level->StartInspecting), &(inspector->WaitInspect));

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

    pthread_mutex_lock(&(inspector->top_level->qLock));
    enqueue(inspector->top_level->q, inspector->value, inspector->top_level->tuple[0]);
    pthread_mutex_unlock(&(inspector->top_level->qLock));

  not_valid:
    pthread_cond_signal(&(inspector->EndInspect));
    pthread_mutex_unlock(&(inspector->WaitInspect));
  }

  pthread_exit(NULL);
}

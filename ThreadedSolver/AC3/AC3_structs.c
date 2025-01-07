#include "AC3_structs.h"
#include "CSP.c"
#include "queue.c"

#include <pthread.h>


/* --------------------------------- DEFINE ARGUMENT STRUCTURES --------------------------------- */

struct AC3 {
  CSP *csp;
  Queue *q;
  int *tuple;
  int slot, neighbor;

  pthread_t parent_thread;
  pthread_t finish_thread;
  pthread_mutex_t qLock;
  pthread_cond_t scanning;

  Revisors **workers;
  pthread_mutex_t revising;
  pthread_cond_t StartRevise;
  int revised;

  HOA **inspectors;
  pthread_mutex_t inspecting;
  pthread_cond_t StartInspecting;

  int restart;
  int completed;
  pthread_cond_t fetched;
};

struct Revisors {
  AC3 *top_level;
  pthread_t thread;
  pthread_mutex_t WaitFetch;
  pthread_mutex_t WaitRevise;
  pthread_cond_t EndRevise;
  int value, satisfied;
};

struct HOA {
  AC3 *top_level;
  pthread_t thread;
  pthread_mutex_t WaitFetch;
  pthread_mutex_t WaitInspect;
  pthread_cond_t EndInspect;
  int value;
};

void check_failure(void *ptr) {
  if (ptr == NULL) {
    fprintf(stderr, "Allocation failed initializing the AC3 struct.\n");
    exit(EXIT_FAILURE);
  }
}


/* -------------------------------------- INITIALIZERS -------------------------------------- */

AC3 *initAC3(CSP *csp, Queue *q, pthread_t *parent, pthread_t *finish) {
  AC3 *a = malloc(sizeof(AC3));
  check_failure((void *)a);

  a->csp = csp;
  a->q = q;

  a->tuple = calloc(2, sizeof(int));
  check_failure((void *)a->tuple);

  a->slot = 0, a->neighbor = 0;

  a->parent_thread = *parent;
  a->finish_thread = *finish;

  pthread_mutex_init(&a->qLock, NULL);
  check_failure((void *)(&a->qLock));

  pthread_cond_init(&a->scanning, NULL);
  check_failure((void *)(&a->scanning));

  a->workers = malloc(NUM_VALUES * sizeof(Revisors));
  check_failure((void *)a->workers);

  for (int i = 0; i < NUM_VALUES; i++) {
    a->workers[i] = initRevisors(a);
    check_failure((void *)a->workers[i]);
  }

  pthread_mutex_init(&a->revising, NULL);
  check_failure((void *)(&a->revising));

  pthread_cond_init(&a->StartRevise, NULL);
  check_failure((void *)(&a->StartRevise));

  a->revised = 0;

  a->inspectors = malloc(NUM_SLOTS * sizeof(HOA));
  check_failure((void *)a->inspectors);

  for (int j = 0; j < NUM_SLOTS; j++) {
    a->inspectors[j] = initHOA(a);
    check_failure((void *)a->inspectors[j]);
  }

  pthread_mutex_init(&a->inspecting, NULL);
  check_failure((void *)(&a->inspecting));

  pthread_cond_init(&a->StartInspecting, NULL);
  check_failure((void *)(&a->StartInspecting));

  a->restart = 0;
  a->completed = 0;

  pthread_cond_init(&a->fetched, NULL);
  check_failure((void *)(&a->fetched));

  return a;
}

Revisors *initRevisors(AC3 *top_level) {
  Revisors *r = malloc(sizeof(Revisors));
  r->top_level = top_level;

  pthread_mutex_init(&r->WaitFetch, NULL);
  pthread_mutex_init(&r->WaitRevise, NULL);
  pthread_cond_init(&r->EndRevise, NULL);

  r->satisfied = 0;
  return r;
}

HOA *initHOA(AC3 *top_level) {
  HOA *h = malloc(sizeof(HOA));
  h->top_level = top_level;

  pthread_mutex_init(&h->WaitFetch, NULL);
  pthread_mutex_init(&h->WaitInspect, NULL);
  pthread_cond_init(&h->EndInspect, NULL);

  return h;
}


/* -------------------------------------- DESTROYERS -------------------------------------- */

void destroyAC3(AC3 *solver) {
  pthread_cond_destroy(&(solver->StartInspecting));
  pthread_cond_destroy(&(solver->StartRevise));
  pthread_cond_destroy(&(solver->scanning));
  pthread_cond_destroy(&(solver->fetched));

  pthread_mutex_destroy(&(solver->inspecting));
  pthread_mutex_destroy(&(solver->revising));
  pthread_mutex_destroy(&(solver->qLock));

  for (int i = 0; i < NUM_SLOTS; i++) {
    destroyHOA(solver->inspectors[i]);
  }

  for (int j = 0; j < NUM_VALUES; j++) {
    destroyRevisors(solver->workers[j]);
  }

  destroyCSP(solver->csp);
  destroyQueue(solver->q);
  free(solver);
  solver = NULL;
}

void destroyRevisors(Revisors *worker) {
  pthread_mutex_destroy(&(worker->WaitFetch));
  pthread_mutex_destroy(&(worker->WaitRevise));
  pthread_cond_destroy(&(worker->EndRevise));
  free(worker);
  worker = NULL;
}

void destroyHOA(HOA *inspector) {
  pthread_mutex_destroy(&(inspector->WaitFetch));
  pthread_mutex_destroy(&(inspector->WaitInspect));
  pthread_cond_destroy(&(inspector->EndInspect));
  free(inspector);
  inspector = NULL;
}

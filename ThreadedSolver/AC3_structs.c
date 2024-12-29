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
};

struct Revisors {
  AC3 *top_level;
  pthread_t thread;
  pthread_mutex_t WaitRevise;
  pthread_cond_t EndRevise;
  int value, satisfied;
};

struct HOA {
  AC3 *top_level;
  pthread_t thread;
  pthread_mutex_t WaitInspect;
  pthread_cond_t EndInspect;
  int value;
};


/* -------------------------------------- INITIALIZERS -------------------------------------- */

AC3 *initAC3(CSP *csp, Queue *q, pthread_t *parent, pthread_t *finish) {
  AC3 *a = malloc(sizeof(AC3));
  a->csp = csp;
  a->q = q;
  a->tuple = calloc(2, sizeof(int));
  a->slot = 0, a->neighbor = 0;

  a->parent_thread = *parent;
  a->finish_thread = *finish;

  pthread_mutex_init(&a->qLock, NULL);
  pthread_cond_init(&a->scanning, NULL);

  a->workers = malloc(sizeof(Revisors));
  for (int i = 0; i < NUM_VALUES; i++) {
    a->workers[i] = initRevisors(a);
  }

  pthread_mutex_init(&a->revising, NULL);
  pthread_cond_init(&a->StartRevise, NULL);
  a->revised = 0;

  a->inspectors = malloc(sizeof(HOA));
  for (int j = 0; j < NUM_SLOTS; j++) {
    a->inspectors[j] = initHOA(a);
  }

  pthread_mutex_init(&a->inspecting, NULL);
  pthread_cond_init(&a->StartInspecting, NULL);

  a->restart = 0;
  a->completed = 0;

  return a;
}

Revisors *initRevisors(AC3 *top_level) {
  Revisors *r = malloc(sizeof(Revisors));
  r->top_level = top_level;

  pthread_mutex_init(&r->WaitRevise, NULL);
  pthread_cond_init(&r->EndRevise, NULL);

  r->satisfied = 0;
  return r;
}

HOA *initHOA(AC3 *top_level) {
  HOA *h = malloc(sizeof(HOA));
  h->top_level = top_level;

  pthread_mutex_init(&h->WaitInspect, NULL);
  pthread_cond_init(&h->EndInspect, NULL);

  return h;
}


/* -------------------------------------- DESTROYERS -------------------------------------- */

void destroyAC3(AC3 *solver) {
  pthread_cond_destroy(&(solver->StartInspecting));
  pthread_cond_destroy(&(solver->StartRevise));
  pthread_cond_destroy(&(solver->scanning));

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
  pthread_mutex_destroy(&(worker->WaitRevise));
  pthread_cond_destroy(&(worker->EndRevise));
  free(worker);
  worker = NULL;
}

void destroyHOA(HOA *inspector) {
  pthread_mutex_destroy(&(inspector->WaitInspect));
  pthread_cond_destroy(&(inspector->EndInspect));
  free(inspector);
  inspector = NULL;
}

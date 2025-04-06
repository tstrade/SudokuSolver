#include "AC3.h"
#include "../CSP/CSP.h"
#include "../Queue/queue.h"
#include "../sudoku_threads.h"

// Global variables
pthread_mutex_t finished_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rflag_mutex = PTHREAD_MUTEX_INITIALIZER;

void *qcontrol(void *arg)
{
  printf("qcontrol: Starting queue control thread.\n");

  if (qinit() == FAILURE)
  {
    printf("\t\tQueue initialization failed!\n");
    goto end;
  }

  while (QSIZE != 0)
  {
    //printf("qcontrol: Locking qtex to dequeue.\n");
    pthread_mutex_lock(&qtex);
    pthread_mutex_lock(&rflag_mutex);
    rflag = FALSE;
    pthread_mutex_unlock(&rflag_mutex);
    dequeue();
    printf("Analyzing (%d, %d)...\n", Xi, Xj);
    pthread_cond_broadcast(&rcond);
    pthread_mutex_unlock(&qtex);
    sched_yield();

    // Wait for all revise threads to finish
    //printf("qcontrol: Waiting for revise threads to finish.\n");
    for (int i = 0; i < NUM_VALUES; i++)
    {
      sem_wait(&revising);
    }
    for (int i = 0; i < NUM_VALUES; i++)
    {
      sem_post(&revising);
    }

    pthread_mutex_lock(&rflag_mutex);
    if (rflag == FALSE)
    {
      printf("qcontrol: rflag is FALSE, continuing to next iteration.\n");
      pthread_mutex_unlock(&rflag_mutex);
      continue;
    }
    pthread_mutex_unlock(&rflag_mutex);

    if (DCOUNT(Xi) == 0)
    {
      printf("qcontrol: DCOUNT(%d) == 0, exiting.\n", Xi);
      EXIT;
    }

    //printf("qcontrol: Broadcasting vcond to wake verify threads.\n");
    pthread_cond_broadcast(&vcond);
    sched_yield();

    // Wait for all verify threads to finish
    //printf("qcontrol: Waiting for verify threads to finish.\n");
    for (int i = 0; i < NUM_NEIGHBORS; i++)
    {
      sem_wait(&verifying);
    }
    for (int i = 0; i < NUM_NEIGHBORS; i++)
    {
      sem_post(&verifying);
    }
  }

  printf("\t\tAC3 solved the %d x %d Sudoku board in just %f seconds!\n",
         NUM_VALUES, NUM_VALUES, difftime(clock(), start) / CLOCKS_PER_SEC);

end:
  printf("qcontrol: Setting finished flag to TRUE.\n");
  pthread_mutex_lock(&finished_mutex);
  finished = TRUE;
  pthread_mutex_unlock(&finished_mutex);
  pthread_cond_broadcast(&rcond);
  pthread_cond_broadcast(&vcond);

  infer_assignment();
  display();

  printf("qcontrol: Exiting queue control thread.\n");
  return NULL;
}

void *revise(void *arg)
{
  struct revisor r = *(struct revisor *)arg;
  uint8_t Xi_val, Xj_val;

  printf("revise: Starting revise thread for box %d.\n", r.self);

  while (TRUE)
  {
    //printf("revise: Locking rtex for box %d.\n", r.self);
    pthread_mutex_lock(&r.rtex);
    while (TRUE)
    {
      //printf("revise: Waiting for rcond for box %d.\n", r.self);
      pthread_cond_wait(&rcond, &r.rtex);
      pthread_mutex_lock(&finished_mutex);
      if (finished)
      {
        printf("revise: Finished flag is TRUE, breaking for box %d.\n", r.self);
        pthread_mutex_unlock(&finished_mutex);
        break;
      }
      pthread_mutex_unlock(&finished_mutex);

      //printf("revise: Waiting for revising semaphore for box %d.\n", r.self);
      sem_wait(&revising);

      if ((Xi_val = csp.domains[Xi][r.self]) == 0)
      {
        //printf("revise: Xi_val is 0 for box %d, skipping.\n", r.self);
        goto revised;
      }

      if ((Xj_val = csp.domains[Xj][r.self]) == 0)
      {
        //printf("revise: Xj_val is 0 for box %d, skipping.\n", r.self);
        goto revised;
      }

      if (constraint(Xi, Xi_val, Xj, Xj_val) == 0)
      {
        printf("revise: Pruning value %d from box %d.\n", Xi_val, Xi);
        prune(Xi, Xi_val);
        pthread_mutex_lock(&rflag_mutex);
        rflag = TRUE;
        pthread_mutex_unlock(&rflag_mutex);
      }

    revised:
      //printf("revise: Posting revising semaphore for box %d.\n", r.self);
      sem_post(&revising);
    }
    pthread_mutex_unlock(&r.rtex);
    if (finished)
      break;
  }
  printf("revise: Exiting revise thread for box %d.\n", r.self);
  pthread_exit(NULL);
}

void *verify(void *arg)
{
  struct verifier v = *(struct verifier *)arg;
  uint8_t nbr;

  //printf("verify: Starting verify thread for neighbor %d.\n", v.self);

  while (TRUE)
  {
    //printf("verify: Locking vtex for neighbor %d.\n", v.self);
    pthread_mutex_lock(&v.vtex);
    while (TRUE)
    {
      //printf("verify: Waiting for vcond for neighbor %d.\n", v.self);
      pthread_cond_wait(&vcond, &v.vtex);
      pthread_mutex_lock(&finished_mutex);
      if (finished)
      {
        printf("verify: Finished flag is TRUE, breaking for neighbor %d.\n", v.self);
        pthread_mutex_unlock(&finished_mutex);
        break;
      }
      pthread_mutex_unlock(&finished_mutex);

      //printf("verify: Waiting for verifying semaphore for neighbor %d.\n", v.self);
      sem_wait(&verifying);

      nbr = neighbors[Xi][v.self];
      if (nbr == Xj)
      {
        //printf("verify: Neighbor %d is Xj, skipping for neighbor %d.\n", nbr, v.self);
        goto verified;
      }

      if (is_present(nbr, Xi) == ABSNT)
      {
        //printf("verify: Neighbor %d is absent, skipping for neighbor %d.\n", nbr, v.self);
        goto verified;
      }

      printf("verify: Enqueuing (%d, %d)...\n", nbr, Xi);
      pthread_mutex_lock(&qtex);
      enqueue(nbr, Xi);
      pthread_mutex_unlock(&qtex);

    verified:
      //printf("verify: Posting verifying semaphore for neighbor %d.\n", v.self);
      sem_post(&verifying);
    }
    pthread_mutex_unlock(&v.vtex);
    if (finished)
      break;
  }
  printf("verify: Exiting verify thread for neighbor %d.\n", v.self);
  pthread_exit(NULL);
}
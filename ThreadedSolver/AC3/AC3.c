#include "AC3.h"
#include "../CSP/CSP.h"
#include "../Queue/queue.h"
#include "../sudoku_threads.h"

void *qcontrol(void *arg)
{
  if (qinit() == FAILURE)
    goto end_fail;

  int rev_val, ver_val;

  while (QSIZE != 0) {
    pthread_mutex_lock(&qtex);
    rflag = FALSE;
    rev_val = ver_val = 0;
    dequeue();
    pthread_cond_broadcast(&rcond);
    pthread_mutex_unlock(&qtex);
    sched_yield();

    do {
      sem_getvalue(&revising, &rev_val);
    } while (rev_val != NUM_VALUES);
      
    if (rflag == FALSE)
      continue;
    
    if (DCOUNT(Xi) == 0)
      EXIT;

    pthread_cond_broadcast(&vcond);
    sched_yield();
    do {
      sem_getvalue(&verifying, &ver_val);
    } while (ver_val != NUM_NEIGHBORS);
  }

  printf("\t\tAC3 solved the %d x %d Sudoku board in just %f seconds!\n", 
    NUM_VALUES, NUM_VALUES, difftime(clock(), start) / CLOCKS_PER_SEC);
  goto end;

  end_fail:
    printf("\t\tQueue initialization failed!\n");
  end:
    finished = TRUE;
    pthread_cond_broadcast(&rcond);
    pthread_cond_broadcast(&vcond);

    infer_assignment();
    display();
    
    return NULL;
}

void *revise(void *arg)
{
  struct revisor r = *(struct revisor *)arg;
  uint8_t Xi_val, Xj_val;

  while (TRUE) {
    pthread_mutex_lock(&r.rtex);
    pthread_cond_wait(&rcond, &r.rtex);
    sem_wait(&revising);

    if (finished == TRUE) 
      break;

    if ((Xi_val = csp.domains[Xi][r.self]) == 0)
      goto revised;

    if ((Xj_val = csp.domains[Xj][r.self]) == 0)
      goto revised;

    if (constraint(Xi, Xi_val, Xj, Xj_val) == 0) {
      prune(Xi, Xi_val);
      rflag = TRUE;
    }

    revised:
      sem_post(&revising);
      pthread_mutex_unlock(&r.rtex);
  }
  sem_post(&revising);
  pthread_mutex_unlock(&r.rtex);
  pthread_exit(NULL);
}

void *verify(void *arg)
{
  struct verifier v = *(struct verifier *)arg;
  uint8_t nbr;

  while (TRUE) {
    pthread_mutex_lock(&v.vtex);
    pthread_cond_wait(&vcond, &v.vtex);
    sem_wait(&verifying);

    if (finished == TRUE) 
      break;

    nbr = neighbors[Xi][v.self];
    if (nbr == Xj)
      goto verified;

    if (is_present(nbr, Xi) == ABSNT)
      goto verified;

    pthread_mutex_lock(&qtex);
    enqueue(nbr, Xi);
    pthread_mutex_unlock(&qtex);
    
    verified:
      sem_post(&verifying);
      pthread_mutex_unlock(&v.vtex);
  }
  sem_post(&verifying);
  pthread_mutex_unlock(&v.vtex);
  pthread_exit(NULL);
}
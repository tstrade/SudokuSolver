#include "AC3.h"
#include "../CSP/CSP.h"
#include "../Queue/queue.h"
#include "../sudoku_threads.h"

void *qcontrol(void *arg)
{
  qinit();
  int rev_val = 0;
  int ver_val = 0;

  while (QSIZE != 0) {
    Q_LOCK;
    rflag = FALSE;
    rev_val = ver_val = 0;
    dequeue();
    R_BDCAST;
    Q_UNLOCK;

    do {
      REV_VAL(&rev_val);
    } while (rev_val != NUM_VALUES);
      
    if (rflag == FALSE)
      continue;
    
    if (DCOUNT(Xi) == 0)
      EXIT;

    V_BDCAST;
    do {
      VER_VAL(&ver_val);
    } while (ver_val != NUM_NEIGHBORS);
  }
  printf("\t\tAC3 solved the %dx%d Sudoku board in just %f seconds!\n", 
    NUM_VALUES, NUM_VALUES, difftime(clock(), start) / CLOCKS_PER_SEC);
  
  finished = TRUE;
  R_BDCAST;
  V_BDCAST;

  infer_assignment();
  display();
  
  return NULL;
}

void *revise(void *arg)
{
  struct revisor r = *(struct revisor *)arg;
  uint8_t Xi_val, Xj_val;
  uint8_t nsatisfied;

  while (TRUE) {
    REV_WAIT;
    R_LOCK(r.self);
    R_WAIT(&r.rtex);

    if (finished == TRUE)
      break;

    if ((Xi_val = csp.domains[Xi][r.self]) == 0)
      goto revised;

    nsatisfied = 0;
    for (uint8_t j = 0; j < NUM_VALUES; j++) {
      if ((Xj_val = csp.domains[Xj][j]) == 0)
        continue;

      nsatisfied += constraint(Xi, Xi_val, Xj, Xj_val);
    }

    if (nsatisfied == 0) {
      prune(Xi, Xi_val);
      rflag = TRUE;
    }

    revised:
    R_UNLOCK(r.self);
    REV_POST;
  }
  R_UNLOCK(r.self);
  REV_POST;
  pthread_exit(NULL);
}

void *verify(void *arg)
{
  struct verifier v = *(struct verifier *)arg;
  uint8_t nbr;

  while (TRUE) {
    VER_WAIT;
    V_LOCK(v.self);
    V_WAIT(&v.vtex);

    if (finished == TRUE)
      break;

    nbr = neighbors[Xi][v.self];
    if (nbr == Xj)
      goto verified;

    if (is_present(nbr, Xi) == ABSNT)
      goto verified;

    Q_LOCK;
    enqueue(nbr, Xi);
    Q_UNLOCK;
    
    verified:
    V_UNLOCK(v.self);
    VER_POST;
  }
  V_UNLOCK(v.self);
  VER_POST;
  pthread_exit(NULL);
}
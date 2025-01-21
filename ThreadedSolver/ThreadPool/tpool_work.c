#include "tpool.h"

/* ---------------------------------------------------------
     tpool_add_work() - Push thread onto queue with
                          specified payload function
   --------------------------------------------------------- */

status tpool_add_work(thread_func_t func, void *arg)
{
  tpid work;

  if (func == NULL)
    return FAILURE;

  LOCK();

  work = TPTAIL();

  if (ISEMPTY(work)) {
    UNLOCK();
    return FAILURE;
  }

  tpool[work].func = func;
  tpool[work].arg = arg;
  tpool[work].tnext = (work + 1) % NUM_THREADS;
  tpooltail = tpool[work].tnext;

  WORK_BROADCAST();
  UNLOCK();

  return SUCCESS;
}


/* -----------------------------------------------------
     tpool_get_work() - Pop thread from queue and
                          update the head
   ----------------------------------------------------- */

tpid tpool_get_work()
{
  tpid work;

  work = TPHEAD();

  if (work == TPTAIL()) {
    tpoolhead = (tpid)TPOOL_EMPTY;
    tpooltail = (tpid)TPOOL_EMPTY;

  } else if (NOTEMPTY(work)) {
    tpoolhead = tpool[work].tnext;
  }

  return work;
}


/* -----------------------------------------------------
     tpool_worker() - Waits for work and distributes
                        to threads in pool
   ----------------------------------------------------- */

void *tpool_worker(void *arg)
{
  tpid work;

  if (arg != NULL)
    return (void *)FAILURE;

  while (1) {

    LOCK();
    while (ISEMPTY(TPHEAD()) && TPCONT())
      WORK_WAIT();

    if (TPSTOP())
      break;

    work = tpool_get_work();
    INCWORK();
    UNLOCK();

    if (NOTEMPTY(work))
      tpool[work].func(tpool[work].arg);

    LOCK();
    DECWORK();

    if (TPSTOP() && NOTWORKING() && ISEMPTY(TPHEAD()))
      WORKING_SIGNAL();
    UNLOCK();
  }

  DECACT();
  WORKING_SIGNAL();
  UNLOCK();

  return (void *)SUCCESS;
}

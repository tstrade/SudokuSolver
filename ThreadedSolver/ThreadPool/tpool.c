#include "tpool.h"

struct tpool_t tpool[NUM_THREADS];

tpid tpool_init()
{
  uint i;

  pthread_attr_init(&work_attr);
  pthread_attr_setdetachstate(&work_attr, PTHREAD_CREATE_DETACHED);

  pthread_mutex_init(&work_mutex, NULL);

  pthread_cond_init(&work_cond, NULL);
  pthread_cond_init(&work_cond, NULL);

  nworking = 0;
  nthreads = NUM_THREADS;
  tpoolstop = 0;

  tpoolhead = (tpid)TPOOL_EMPTY;
  tpooltail = (tpid)TPOOL_EMPTY;

  for (i = 0; i < NUM_THREADS; i++)
    pthread_create(&(tpool[i].thread), &work_attr, tpool_worker, NULL);

  return TPHEAD();
}

status tpool_destroy()
{
  tpid work;

  LOCK();

  tpoolhead = (tpid)TPOOL_EMPTY;
  tpooltail = (tpid)TPOOL_EMPTY;
  tpoolstop = 1;

  WORK_BROADCAST();
  UNLOCK();

  tpool_wait();

  work = 0;
  while (work != NUM_THREADS)
    pthread_cancel(tpool[work].thread);

  pthread_attr_destroy(&work_attr);
  pthread_mutex_destroy(&work_mutex);
  pthread_cond_destroy(&work_cond);
  pthread_cond_destroy(&working_cond);

  return SUCCESS;
}

void tpool_wait()
{
  LOCK();

  while(1) {
    if (NOTEMPTY(TPHEAD()) || (TPCONT() && WORKING()) || (TPCONT() && ACTIVE()))
      WORKING_WAIT();
    else
      break;
  }

  UNLOCK();
}

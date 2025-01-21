#ifndef TPOOL_H
#define TPOOL_H

#include "../soduku_threads.h"

#define NUM_THREADS (NUM_SLOTS + NUM_NEIGHBORS + NUM_VALUES)
#define TPOOL_EMPTY (-1)

typedef void (*thread_func_t)(void *arg);
typedef int64_t tpid;

struct tpool_t {
  thread_func_t func;
  void *arg;
  pthread_t thread;
  tpid tnext;
};

extern struct tpool_t tpool[];

extern pthread_attr_t work_attr;
extern pthread_mutex_t work_mutex;
extern pthread_cond_t work_cond;
extern pthread_cond_t working_cond;

extern size_t nworking;
extern size_t nthreads;
extern status tpoolstop;
extern tpid tpoolhead;
extern tpid tpooltail;

tpid tpool_init();
status tpool_destroy();
void tpool_wait();

status tpool_add_work(thread_func_t func, void *arg);
tpid tpool_get_work();
void *tpool_worker(void *arg);

#define LOCK() (pthread_mutex_lock(&work_mutex))
#define UNLOCK() (pthread_mutex_unlock(&work_mutex))

#define WORK_BROADCAST() (pthread_cond_broadcast(&work_cond))
#define WORK_WAIT() (pthread_cond_wait(&work_cond, &work_mutex))
#define WORK_SIGNAL() (pthread_cond_signal(&work_cond))

#define WORKING_WAIT() (pthread_cond_wait(&working_cond, &work_mutex))
#define WORKING_SIGNAL() (pthread_cond_signal(&working_cond))

#define TPHEAD() (tpoolhead)
#define TPTAIL() (tpooltail)

#define TPSTOP() (tpoolstop)
#define TPCONT() (!tpoolstop)

#define WORKING() (nworking)
#define INCWORK() (nworking++)
#define DECWORK() (nworking--)
#define NOTWORKING() (nworking == 0)

#define ACTIVE() (nthreads)
#define INCACT() (nthreads++)
#define DECACT() (nthreads--)

#define ISEMPTY(x) ((x) == TPOOL_EMPTY)
#define NOTEMPTY(x) ((x) != TPOOL_EMPTY)

#endif

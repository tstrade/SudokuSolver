#ifndef AC3_H
#define AC3_H

#include "../sudoku_threads.h"

extern pthread_t parent_thread;
extern pthread_t final_thread;
typedef uint8_t  tid8;

/********************************************************************/
/*                                                                  */
/* Globals to protect: queue[], in_q[], Xi, Xj, qhead/tail/size     */
/*                     csp, assignments[]                           */
/*                                                                  */
/* Producers: AC3    (enqueue, dequeue, infer_assignment)           */
/*            revise (prune)                                        */
/*                                                                  */
/* Consumers: AC3    (csp.domains, in_q[])                          */
/*            revise (csp.domains)                                  */
/*                                                                  */
/* Num. Loop: neighbors = 20 per slot, csp.domains = 10 per slot    */
/*                                                                  */
/********************************************************************/


/* ---------------------- How to split tasks ---------------------- */

/********************************************************************/
/*                                                                  */
/* AC3 - call dequeue() until empty, call revise(), check DCOUNT,   */
/*       check domain (10 loops), check neighbor (20 loops),        */
/*       call is_present(), call enqueue() is last 3 satisfied      */
/*                                                                  */
/********************************************************************/

/* 1. qcontrol() - call dequeue(), enqueue() (Lock queue[])         */
/*    - Flags to determine if tuple should be enqueued:             */
/*      -> pruned    = TRUE / FALSE (Lock csp.domains)              */
/*      -> notempty  = TRUE / FALSE (Lock csp.domains)              */
/*      (Checks following flags with NUM_SLOTS threads)             */
/*      -> duplicate = FALSE / TRUE                                 */
/*      -> neighbor  = TRUE / FALSE                                 */
/*      -> queued    = FALSE / TRUE (Lock in_q[])                   */
void *qcontrol(void *arg);
extern uint8_t finished;

/* 2. verify_tuple() - check and set flags for 3 conditions         */
/*      - duplicate: Does Xj match the proposed slot to enqueue?    */
/*      - neighbor:  Is the proposed slot a neighbor of Xj?         */
/*      - queued:    Are Xj and the proposed slot already queued?   */
void *verify(void *arg);


/********************************************************************/
/*                                                                  */
/* revise - check csp.domains for Xi and Xj (100 loops), call       */
/*          prune() if constraint not satisfied, return status      */
/*                                                                  */
/********************************************************************/

/* Workload can simply be split between NUM_VALUES threads          */
void *revise(void *arg);


/* Protections for global variables */

extern pthread_mutex_t qtex;  /* Queue mutex */
#define Q_LOCK   (pthread_mutex_lock(&qtex))
#define Q_UNLOCK (pthread_mutex_unlock(&qtex))

extern pthread_mutex_t dtex;  /* Domain mutex */
#define D_LOCK   (pthread_mutex_lock(&dtex))
#define D_UNLOCK (pthread_mutex_unlock(&dtex))

extern pthread_cond_t rcond;  /* Condition var. for revise */
#define R_WAIT(x) (pthread_cond_wait(&rcond, (x)))
#define R_BDCAST  (pthread_cond_broadcast(&rcond))

extern pthread_cond_t vcond;  /* Condition var. for verify */
#define V_WAIT(x) (pthread_cond_wait(&vcond, (x)))
#define V_BDCAST  (pthread_cond_broadcast(&vcond))

extern sem_t revising;  /* Num. threads checking constraints */
#define REV_WAIT   (sem_wait(&revising))
#define REV_POST   (sem_post(&revising))
#define REV_VAL(x) (sem_getvalue(&revising, (x)))

extern sem_t verifying;  /* Num. threads verifying tuple */
#define VER_WAIT   (sem_wait(&verifying))
#define VER_POST   (sem_post(&verifying))
#define VER_VAL(x) (sem_getvalue(&verifying, (x)))

status ac3init();
status ac3free();

/* Each thread needs mutex for condition and value it's responsible for  */
struct revisor {
    tid8 self;
    pthread_mutex_t rtex;
};
extern struct revisor rtab[];  
extern uint8_t rflag;
#define R_LOCK(x)   (pthread_mutex_lock(&rtab[(x)].rtex))
#define R_UNLOCK(x) (pthread_mutex_unlock(&rtab[(x)].rtex)) 
status rinit();
status rfree();

struct verifier {
    tid8 self;
    pthread_mutex_t vtex;
};
extern struct verifier vtab[];
status vinit();
status vfree();

#endif

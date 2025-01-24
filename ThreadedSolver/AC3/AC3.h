#ifndef AC3_H
#define AC3_H

#include "../sudoku_threads.h"

extern pthread_t parent_thread;
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

extern pthread_mutex_t qtex;    /* Queue mutex               */
extern pthread_cond_t rcond;    /* Condition var. for revise */
extern pthread_cond_t vcond;    /* Condition var. for verify */
extern sem_t revising;  /* Num. threads checking constraints */
extern sem_t verifying; /* Num. threads verifying tuple      */

status ac3init();
status ac3free();

/* Each thread needs mutex for condition and value it's responsible for  */
struct revisor {
    tid8 self;
    pthread_mutex_t rtex;
};

extern struct revisor rtab[];  
extern uint8_t rflag;
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

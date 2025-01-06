#ifndef QUEUE_H
#define QUEUE_H

#include "soduku_threads.h"

#define FRNT_MASK 0xFFFF0000
#define BACK_MASK 0x0000FFFF
#define TUP1_MASK 0x0000FF00
#define TUP2_MASK 0x000000FF

typedef int qid;

struct qentry {
  uint qdata;
  uint qends;
  uint qlink;
};

extern struct qentry queue[];
extern ushort Xi, Xj;
extern qid currhead, currsize;
int initQueue();

#define QSIZE() (currsize)
#define QHEAD() (currhead)
#define QTAIL() ((queue[QHEAD()].qends & BACK_MASK)      )
#define QNEXT() ((queue[QHEAD()].qlink & FRNT_MASK) >> 16)
#define QTUP1() ((queue[QHEAD()].qdata & FRNT_MASK) >> 16)
#define QTUP2() ((queue[QHEAD()].qdata & BACK_MASK)      )



/* ----- Queue Operations ----- */
int enqueue(ushort i, ushort j);
qid dequeue();

#endif

#ifndef QUEUE_H
#define QUEUE_H

#include "../soduku_threads.h"

#define TMASK 0x00FF
#define EMPTY     -1
#define PRSNT      2
#define ABSNT      3

typedef int qid;

struct qentry {
  ushort qdata;
  ushort qnext;
};

extern struct qentry queue[];
extern ushort Xi, Xj;
extern qid qhead, qsize, qtail;

#define QSIZE() (qsize)
#define QHEAD() (qhead)
#define QTAIL() (qtail)
#define QMSK(x) ((uint)1 << x)

extern uint in_q[];

/* ----- Queue Operations ----- */
status qinit();
status enqueue(ushort i, ushort j);
status dequeue();
status ispresent(ushort i, ushort j);

#endif

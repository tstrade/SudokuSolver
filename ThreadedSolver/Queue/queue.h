#ifndef QUEUE_H
#define QUEUE_H

#include "../soduku_threads.h"
#include "../CSP/CSP.h"

#define EMPTY      0
#define PRSNT      1
#define ABSNT      2

typedef int qid16;

struct qentry {
  uint8_t xi, xj;
  qid16 qnext;
};

extern struct qentry queue[];
extern uint32_t in_q[];
#define QMSK(i, j) (1 << neighbor_idx((i), (j)))

extern uint8_t Xi, Xj;
extern qid16 qhead, qsize, qtail;

#define QSIZE()    (qsize)
#define INCSIZE()  (qsize++)
#define DECSIZE()  (qsize--)

#define QHEAD()    (qhead)
#define SETHEAD(x) (qhead = (x))

#define QTAIL()    (qtail)
#define SETTAIL(x) (qtail = (x))

/* ----- Queue Operations ----- */
status qinit();
status enqueue(uint8_t i, uint8_t j);
status dequeue();
status is_present(uint8_t i, uint8_t j);

#endif
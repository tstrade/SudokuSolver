#include "queue.h"

status dequeue()
{
  qid16 oldhead, newhead;

  /* Obtain the index of the new queue head */
  oldhead = QHEAD;
  newhead = queue[oldhead].qnext;

  /* Collect the data and store in the globals Xi, Xj */
  Xi = queue[oldhead].xi;
  Xj = queue[oldhead].xj;

  /* Update queue and table of present tuples */
  SETHEAD(newhead);
  DECSIZE;
  in_q[Xi] &= (~QMSK(Xi, Xj));

  if (QSIZE <= 0)
    return EMPTY;

  return SUCCESS;
}

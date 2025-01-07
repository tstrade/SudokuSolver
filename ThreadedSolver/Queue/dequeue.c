#include "queue.h"

ushort Xi = 0;
ushort Xj = 0;

status dequeue()
{
  ushort oldhead, newhead;

  if (QSIZE() == 0)
    return EMPTY;

  /* Obtain the index of the new queue head */
  oldhead = QHEAD();
  newhead = queue[oldhead].qnext;

  /* Collect the data and store in the globals Xi, Xj */
  Xi = (((queue[oldhead].qdata) >> 8) & TMASK);
  Xj = ((queue[oldhead].qdata) & TMASK);

  /* Update queue and table of present tuples */
  qhead = newhead;
  qsize--;
  in_q[Xi] &= (~QMSK(Xj));

  return SUCCESS;
}

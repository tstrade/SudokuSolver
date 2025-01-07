#include "queue.h"

ushort Xi = 0;
ushort Xj = 0;

status enqueue(ushort i, ushort j)
{
  ushort maxsize, oldtail, newtail, data;

  if (i == j)
    return FAILURE;

  /* Store data inside single variable */
  data = ((i << 8) | j);

  /* Get index for the new tail of the queue */
  maxsize = NUM_SLOTS * NUM_NEIGHBORS;
  oldtail = QTAIL();
  newtail = ((oldtail + 1) % maxsize);

  /* Store data in current tail and update queue */
  queue[oldtail].qdata = data;
  queue[oldtail].qnext = newtail;
  qsize++;
  qtail = newtail;
  in_q[i] |= QMSK(j);

  return SUCCESS;
}

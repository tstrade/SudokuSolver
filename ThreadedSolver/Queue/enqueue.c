#include "queue.h"

status enqueue(uint8_t i, uint8_t j)
{
  uint16_t maxsize;
  qid16 oldtail, newtail;

  if (i == j)
    return FAILURE;

  /* Get index for the new tail of the queue */
  maxsize = NUM_SLOTS * NUM_NEIGHBORS;
  oldtail = QTAIL();
  newtail = ((oldtail + 1) % maxsize);

  /* Store data in current tail and update queue */
  queue[oldtail].xi = i;
  queue[oldtail].xj = j;
  queue[oldtail].qnext = newtail;

  INCSIZE();
  SETTAIL(newtail);

  in_q[i] |= (1 << j);

  return SUCCESS;
}

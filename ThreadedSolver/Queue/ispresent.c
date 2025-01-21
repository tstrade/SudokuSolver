#include "queue.h"

status ispresent(uint8_t i, uint8_t j)
{
  if (in_q[i] & QMSK(i, j))
    return PRSNT;

  return ABSNT;
}
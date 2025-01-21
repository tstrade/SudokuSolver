#include "queue.h"

status is_present(uint8_t i, uint8_t j)
{
  if (in_q[i] & QMSK(i, j))
    return PRSNT;

  return ABSNT;
}
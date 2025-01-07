#include "queue.h"

status ispresent(ushort i, ushort j)
{
  if (in_q[i] & QMSK(j))
    return PRSNT;

  return ABSNT;
}

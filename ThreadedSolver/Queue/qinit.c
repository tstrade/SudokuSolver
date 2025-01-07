#include "queue.h"

uint in_q[NUM_SLOTS];

struct qentry queue[NUM_SLOTS * NUM_NEIGHBORS];
qid qhead = 0;
qid qsize = 0;
qid qtail = 0;

status qinit()
{
  int slot, neighbor;
  for (slot = 0; slot < NUM_SLOTS; slot++)
    for (neighbor = 0; neighbor < NUM_NEIGHBORS; neighbor++)
      if (enqueue(slot, neighbors[slot][neighbor]) == FAILURE)
        return FAILURE;

  return SUCCESS;
}

#include "queue.h"

struct qentry queue[NUM_SLOTS * NUM_NEIGHBORS];
uint32_t in_q[NUM_SLOTS];

uint8_t Xi = 0;
uint8_t Xj = 0;

qid16 qhead = 0;
qid16 qsize = 0;
qid16 qtail = 0;

status qinit()
{
  for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
    for (uint8_t neighbor = 0; neighbor < NUM_NEIGHBORS; neighbor++) {
      if (enqueue(slot, neighbors[slot][neighbor]) == FAILURE)
        return FAILURE;
    }
  }
  return SUCCESS;
}

#include "queue.h"

struct qentry queue[NUM_SLOTS * NUM_NEIGHBORS];

int initQueue() {
  int slot, neighbor;
  for (slot = 0; slot < NUM_SLOTS; slot++)
    for (neighbor = 0; neighbor < NUM_NEIGHBORS; neighbor++)
      if (enqueue(slot, neighbors[slot][neighbor]) == FAILURE)
        return FAILURE;

  return SUCCESS;
}

int enqueue(ushort i, ushort j) {
  ushort old_tail;
  uint qdata;

  if (i == j)
    return FAILURE;

  old_tail = QTAIL();
  qdata = (i << 16);
  qdata |= j;

  queue[old_tail].qdata = qdata;


  return SUCCESS;
}

qid dequeue() {
  return (qid)SUCCESS;
}

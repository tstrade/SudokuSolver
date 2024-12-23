#include "qbit.h"

#include <stdio.h>
#include <stdlib.h>

struct Qbit {
  unsigned short Qmask;
  Qbit *next;
};

Qbit *initQbit(ushort idx, ushort neighbor) {
  Qbit *qbit = malloc(sizeof(Qbit));
  qbit->Qmask = (1 << neighbor) | idx;

  return qbit;
}

void destroyQbit(Qbit *qbit) {
  free(qbit);
  qbit = NULL;
}

void enqueue(uint *inQueue, Qbit *qbit, ushort idx, ushort neighbor) {
  inQueue[CURR_SIZE]++;
  inQueue[idx] |= ((unsigned)1 << neighbor);

  if (qbit == NULL) { qbit = initQbit(idx, neighbor); return; }

  while (qbit->next != NULL) { qbit = qbit->next; }
  qbit->next = initQbit(idx, neighbor);
}

Qbit *dequeue(uint *inQueue, Qbit *qbit, ushort *tuple) {
  ushort neighbor = qbit->Qmask & GET_NEIGHBOR;
  ushort idx = qbit->Qmask & GET_IDX;
  Qbit *head = qbit->next;
  destroyQbit(qbit);

  inQueue[CURR_SIZE]--;
  inQueue[idx] &= (~(1 << neighbor));
  ushort n_integer = 0;
  while (neighbor > 0) {
    neighbor = neighbor >> 1;
    n_integer++;
  }
  tuple[0] = n_integer;
  tuple[1] = idx;

  return head;
}

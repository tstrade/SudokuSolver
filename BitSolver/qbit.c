#include "qbit.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

ulong *initQbit(ushort idx, ushort neighbor) {
  ulong *qbit = calloc(NUM_SLOTS + METADATA, sizeof(ulong));
  ulong *data = calloc(1, sizeof(ulong));
  ulong *tail = calloc(1, sizeof(ulong));

  qbit[HEAD_META] = (ulong)data; // Contains tuple and address of next item
  qbit[TAIL_META] = (ulong)tail; // Contains address of last item only

  ulong mask = (neighbor << SHIFT_NEIGHBOR) | idx;
  *data = (mask << SHIFT_MASK) | (ulong)tail;
  qbit[SIZE_META]++;

  return qbit;
}

ulong *enqueue(ulong *qbit, ushort idx, ushort neighbor) {
  if (qbit == NULL) { qbit = initQbit(idx, neighbor); return qbit; }
  qbit[SIZE_META]++;
  // Add flag that neighbor for slot idx has been queued
  qbit[idx] |= (1 << neighbor);

  // Same process as initializing the head
  ulong *oldTail;
  oldTail = (ulong *)qbit[TAIL_META];

  ulong *newTail = calloc(1, sizeof(ulong));
  qbit[TAIL_META] = (ulong)newTail;

  ulong mask = ((1 << neighbor) << SHIFT_NEIGHBOR) | idx;
  *oldTail = (mask << SHIFT_MASK) | (ulong)newTail;

  return qbit;
}

ulong dequeue(ulong *qbit) {
  ulong *head = (ulong *)qbit[HEAD_META];
  ulong tuple = (ulong)(*head >> SHIFT_MASK);
  ulong *next = (ulong *)(*head & GET_NEXT);

  qbit[HEAD_META] = (ulong)next;
  free(head);

  qbit[SIZE_META]--;

  return tuple;
}

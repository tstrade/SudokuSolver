#include "qbit.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

uint *initQbit(ushort idx, ushort neighbor) {
  uint *qbit = calloc(NUM_SLOTS + METADATA, sizeof(uint));

  ulong *data = calloc(1, sizeof(ulong));
  *data = ((1 << neighbor) << SHIFT_NEIGHBOR) | idx;
  *data = ((ulong)data << sizeof(uint));

  qbit[HEAD_META] = (uint)(*data >> sizeof(uint));
  qbit[SIZE_META]++;

  return qbit;
}

uint *enqueue(uint *qbit, ushort idx, ushort neighbor) {
  if (qbit == NULL) { qbit = initQbit(idx, neighbor); return qbit; }

  qbit[SIZE_META]++;
  qbit[idx] |= (1 << neighbor);

  // MS half points to the next item; LS half contains the item
  qbit[NEXT_META] = qbit[TAIL_META];
  ulong *next = calloc(1, sizeof(ulong));
  *next = (neighbor << 7) | idx;
  qbit[TAIL_META] = (uint)(*next >> sizeof(uint));
  return qbit;
}

ushort dequeue(uint *qbit) {
  ulong *data = NULL;
  data += qbit[HEAD_META];
  qbit[HEAD_META] = qbit[NEXT_META];

  ushort neighbor = *data & GET_NEIGHBOR;
  ushort idx= *data & GET_IDX;
  free(data);

  ulong *next = NULL;
  next += qbit[NEXT_META];
  qbit[NEXT_META] = (uint)(*next >> sizeof(uint));

  qbit[SIZE_META]--;
  qbit[idx] ^= (~(1 << neighbor));
  return ((neighbor << SHIFT_NEIGHBOR) | idx);
}

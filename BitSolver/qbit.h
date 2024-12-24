#ifndef QBIT_H
#define QBIT_H

#define GET_NEXT 0x0000FFFFFFFFFFFF
#define GET_IDX 0x007F
#define GET_NEIGHBOR 0x7F00
#define SHIFT_NEIGHBOR 8
#define SHIFT_MASK 48

#define SIZE_META 81
#define HEAD_META 82
#define TAIL_META 83
#define METADATA 3

#ifndef NUM_SLOTS
#define NUM_SLOTS 81
#endif

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

ulong *initQbit(ushort idx, ushort neighbor);
void destroyQbit(ulong *qbit);
ulong *enqueue(ulong *qbit, ushort idx, ushort neighbor);
ulong dequeue(ulong *qbit);

#endif

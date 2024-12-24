#ifndef QBIT_H
#define QBIT_H

#define GET_IDX 0x007F
#define GET_NEIGHBOR 0x0F80
#define SHIFT_NEIGHBOR 7
#define GET_NEXT 0xFFFFFFFF00000000
#define NEXT_MASK 0x000000000000FFFF

#define SIZE_META 81
#define HEAD_META 82
#define TAIL_META 83
#define NEXT_META 84
#define METADATA 4

#ifndef NUM_SLOTS
#define NUM_SLOTS 81
#endif

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

uint *initQbit(ushort idx, ushort neighbor);
void destroyQbit(uint *qbit);
uint *enqueue(uint *qbit, ushort idx, ushort neighbor);
ushort dequeue(uint *qbit);

#endif

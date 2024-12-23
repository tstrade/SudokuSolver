#ifndef QBIT_H
#define QBIT_H

#define GET_IDX 0x007F
#define GET_NEIGHBOR 0x0F80
#define CURR_SIZE 31

typedef struct Qbit Qbit;
typedef unsigned short ushort;
typedef unsigned int uint;

Qbit *initQbit(ushort idx, ushort neighbor);
void destroyQbit(Qbit *qbit);
void enqueue(uint *inQueue, Qbit *qbit, ushort idx, ushort neighbor);
Qbit *dequeue(uint *inQueue, Qbit *qbit, ushort *tuple);

#endif

#ifndef CSP_BITS_H
#define CSP_BITS_H

#include "qbit.h"

#define NUM_VALUES 9
#define NUM_SLOTS 81
#define NUM_NEIGHBORS 20
#define NUM_BOX 3

#define LOG_2 0x7F800000
#define BIAS 127
#define SHIFT_EXP 23

typedef struct CSP CSP;

void initNeighbors(CSP *csp);
CSP *initCSP();
ushort nconflicts(CSP *csp, ushort slot, ushort value);
void display(CSP *csp);
ushort goal_test(CSP *csp);
void prune(CSP *csp, ushort slot, ushort value);
void infer_assignment(CSP *csp);
ushort constraint(ushort A_slot, ushort A_value, ushort B_slot, ushort B_value);
ushort countEntries(ushort *sequence, ushort size);
ushort countBits(ushort sequence);
ushort getRow(ushort slot);
ushort getCol(ushort slot);
ushort isVariable(CSP *csp, ushort slot);
ushort getlog(ushort n);
void destroyCSP(CSP *csp);

#endif

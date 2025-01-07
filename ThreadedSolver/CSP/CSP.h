#ifndef CSP_H
#define CSP_H

#include "../soduku_threads.h"

#define ISVAR 1
#define NTVAR 0

struct CSP {
  ushort nassigns;
  ushort domains[NUM_SLOTS][NUM_VALUES];
  ushort variables[];
};

extern struct CSP csp;
extern ushort neighbors[NUM_SLOTS][NUM_NEIGHBORS];
extern ushort assignments[NUM_SLOTS];


#define constraint(A, a, B, b) ((A != B) && (a != b))

status binit(char *initialPositions);
status ninit();

void assign(ushort variable, ushort value);
void unassign(ushort variable);
ushort nconflicts(ushort variable, ushort value);

void support_pruning();
void suppose(ushort variable, ushort value);
void prune(ushort variable, ushort value);
void infer_assignment();
ushort count(ushort *seq, ushort size);

#define ROW(x) ((x) / NUM_VALUES);
#define COL(x) ((x) % NUM_VALUES);
status is_var(ushort variable);

/* ----- Suplementary / Debugging Functions ----- */
void showVars();
void showCurrentDomains();
void showVariableDomain(ushort variable);
void showRemovals();
void display();

#endif

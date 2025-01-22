#ifndef CSP_H
#define CSP_H

#include "../sudoku_threads.h"

#define ISVAR      1
#define NTVAR      0
#define UNASSIGNED 0

#ifndef NUM_VALUES
#define NUM_VALUES 9
#endif

#ifndef NUM_SLOTS
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#endif

#ifndef NUM_NEIGHBORS
#define NUM_NEIGHBORS ((NUM_VALUES - 1) + (2 * (NUM_VALUES - (NUM_VALUES / 3))))
#endif

#ifndef NUM_BOX
#define NUM_BOX (NUM_VALUES / 3)
#endif

struct CSP {
  uint8_t nassigns, nvars;
  uint8_t domains[NUM_SLOTS][NUM_VALUES + 1];
  uint8_t *variables;
};

extern struct CSP csp;
extern uint8_t neighbors[NUM_SLOTS][NUM_NEIGHBORS];
extern uint8_t assignments[NUM_SLOTS];

#define DCMETA    0
#define DCOUNT(x) (csp.domains[(x)][DCMETA])

#define constraint(A, a, B, b) ((A != B) && (a != b))

#define ASCII_0 48
#define ASCII_9 57

status binit(char *initialPositions);
status ninit();
void bfree();

void assign(uint8_t variable, uint8_t value);
void unassign(uint8_t variable);

uint8_t nconflicts(uint8_t variable, uint8_t value);
uint8_t count(uint8_t *seq, uint8_t size);
status is_var(uint8_t variable);
uint8_t neighbor_idx(uint8_t slot, uint8_t nbr);
status is_neighbor(uint8_t slot, uint8_t nbr);

void support_pruning();
void prune(uint8_t variable, uint8_t value);
void infer_assignment();

#define ROW(x) ((x) / NUM_VALUES);
#define COL(x) ((x) % NUM_VALUES);

/* ----- Suplementary / Debugging Functions ----- */
void showVars();
void showCurrentDomains();
void showVariableDomain(uint8_t variable);
void display();

#endif

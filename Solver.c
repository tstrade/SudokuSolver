#include "CSP.h"
#include "datastructs.h"
#include <stdlib.h>

typedef struct Solver Solver;
typedef struct AC3_args AC3_args;
int var_AC3(AC3_args args);
int AC3(Solver *self, CSP *csp, Queue *q, int **removals);
int revise(struct Solver *self, CSP *csp, int Xi, int Xj, int **removals);
void backtracking_search(Solver *self, CSP *csp);
int *backtrack(Solver *self, int *assignment, CSP *csp);
int select_unassigned_variable(Solver *self, int *assignment, CSP *csp);
Queue *get_queue(Solver *self, CSP *csp, int variable);


struct Solver {
  int (*var_AC3)(AC3_args args);
  int (*revise)(Solver *self, CSP *csp, int Xi, int Xj, int **removals);
  void (*backtracking_search)(Solver *self, CSP *csp);
  int* (*backtrack)(Solver *self, int *assigment, CSP *csp);
  int (*select_unassigned_variable)(Solver *self, int *assignment, CSP *csp);
  Queue* (*get_queue)(Solver *self, CSP *csp, int variable);
};

struct AC3_args {
  Solver *self;
  CSP *csp;
  Queue *q;
  int **removals;
};

int var_AC3(AC3_args args) {
  Solver *self = args.self;
  CSP *csp = args.csp;
  Queue *q = args.q;
  int **rmv = malloc(81 * sizeof(int *));
  if (rmv == NULL) { return -1; }
  for (int i = 0; i < 9; i++) {
    rmv[i] = malloc(9 * sizeof(int));
    if (rmv[i] == NULL) { return -1; }
  }
  int **removals = args.removals ? args.removals : rmv;
  return AC3(self, csp, q, removals);
}

int AC3(Solver *self, CSP *csp, Queue *q, int **removals) {
  if (!q->currSize) {
    // queue = [Neighbors for Variable in csp.curr_domains for Neighbors in self.get_queue(csp, Variable)]
  }
  while (q != NULL) {
    /*
    Xi, Xj = queue[0][0], queue.pop(0)[1]
    if self.revise(csp, Xi, Xj, removals):
        if not csp.curr_domains[Xi]: return False
        for Domain in csp.curr_domains:
            if Xi in csp.neighbors[Domain] and (Domain, Xi) not in queue and Domain != Xj:
                queue.append((Domain, Xi))
    */
  }
  return 1;
}

int main() {
  CSP *csp = initCSP();
}

#include "ABitsC3.h"
#include "qbit.c"
#include "CSP_Bits.c"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int ABitsC3(ulong *qbit, CSP *csp) {
  for (ushort slot = 0; slot < NUM_SLOTS; slot++) {
    for (ushort n = 0; n < NUM_NEIGHBORS; n++) { qbit = enqueue(qbit, slot, csp->neighbors[slot][n]); } }

  ushort Xi, Xj, tuple, neighbor;
  while (qbit[SIZE_META]) {
    tuple = (ushort)dequeue(qbit);
    Xi = tuple >> SHIFT_NEIGHBOR;
    Xj = tuple & GET_IDX;


    if (revise(csp, Xi, Xj)) {
      if (!countBits(csp->domains[Xi])) { printf("ABitsC3 failed!\n"); return 0; }

      for (ushort slot = 0; slot < NUM_SLOTS; slot++) {
	if (!(slot ^ Xi)) { continue; } else { neighbor = 0; }

	  do  { if (!(Xi ^ csp->neighbors[slot][neighbor])) { goto is_neighbor; }
	  neighbor++; } while(neighbor ^ NUM_NEIGHBORS); continue;

      is_neighbor:
	if (qbit[Xi] & (1 << neighbor)) { continue; }
	enqueue(qbit, slot, Xi);
      } // End for loop
    } // End revise
  } // End while loop

  infer_assignment(csp);

  return 1;
}

int revise(CSP *csp, ushort Xi, ushort Xj) {
  ushort revised = 0, satisfied = 0;
  ushort XjValue = csp->domains[Xi], XiValue = csp->domains[Xj];
  ushort Xj_mask = 0x0100, Xi_mask = 0x0100;

  while (Xi_mask) {
    if (!(XiValue & Xi_mask)) { Xi_mask >>= 1; continue; }
    Xj_mask = 0x100;
    while (Xj_mask) {
      if (!(XjValue & Xj_mask)) { Xj_mask /= 2; continue; }

      satisfied += constraint(Xi, getlog(Xi_mask), Xj, getlog(Xj_mask));
      Xj_mask >>= 1;
    }

    if (!satisfied) { csp->domains[Xi] &= (~Xi_mask); revised = 1; }

    Xi_mask >>= 1;
  }
  printf("Revision = %d\n", revised);
  return revised;
}

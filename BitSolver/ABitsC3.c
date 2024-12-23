#include "ABitsC3.h"
#include "qbit.c"
#include "CSP_Bits.c"

int ABitsC3(uint *inQueue, CSP *csp) {
  Qbit *qbit = NULL;
  if (!inQueue[CURR_SIZE]) {
    for (ushort slot = 0; slot < NUM_SLOTS; slot++) {
      for (ushort n = 0; n < NUM_NEIGHBORS; n++) {
	enqueue(inQueue, qbit, slot, n);
      }
    }
  }

  ushort *X = calloc(2, sizeof(ushort));
  ushort neighbor;
  while (inQueue[CURR_SIZE]) {
    qbit = dequeue(inQueue, qbit, X);

    if (revise(csp, X) == 1) {
      if (!countBits(csp->domains[X[0]])) { printf("ABitsC3 failed!\n"); return 0; }

      for (ushort domain = 0; domain < NUM_SLOTS; domain++) {
	if (!(domain ^ X[0])) { continue; }
	neighbor = 0;

	while((neighbor ^ NUM_NEIGHBORS)) {
	  if (!(X[0] ^ csp->neighbors[domain][neighbor])) { goto is_neighbor; }
	  neighbor++;
	}
	continue;

      is_neighbor:
        if (((uint)1 << domain) & inQueue[X[0]]) { continue; }
	enqueue(inQueue, qbit, domain, X[0]);
      } // End for loop
    } // End revise
  } // End while loop

  free(X);
  X = NULL;

  infer_assignment(csp);

  return 1;
}

int revise(CSP *csp, ushort *X) {
  ushort revised = 0, satisfied = 0;
  ushort XjValue = csp->domains[X[1]], XiValue = csp->domains[X[0]];
  ushort XjValue_mask, XiValue_mask = 0x0100;

  while (XiValue_mask) {
    if (!(XiValue & XiValue_mask)) { XiValue_mask >>= 1; continue; }
    XjValue_mask = 0x0100;

    while (XjValue_mask) {
      if (!(XjValue & XjValue_mask)) { XjValue_mask >>= 1; continue; }

      satisfied += constraint(X[0], XiValue, X[1], XjValue);
    }

    if (!satisfied) { prune(csp, X[0], XiValue); revised = 1; }
  }
  return revised;
}

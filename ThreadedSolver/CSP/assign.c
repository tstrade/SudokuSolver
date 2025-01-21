#include "CSP.h"

void assign(uint8_t variable, uint8_t value)
{
  assignments[variable] = value;
  csp.nassigns++;
}

void unassign(uint8_t variable)
{
  assignments[variable] = 0;
}

uint8_t nconflicts(uint8_t variable, uint8_t value)
{
  uint8_t neighbor, n_idx, conflicts = 0;
  for (n_idx = 0; n_idx < NUM_NEIGHBORS; n_idx++) {
    neighbor = neighbors[variable][n_idx];
    if (constraint(variable, value, neighbor, assignments[n_idx]) == 0)
      conflicts++;
  }
  return conflicts;
}

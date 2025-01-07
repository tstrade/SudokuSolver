#include "CSP.h"

void assign(ushort variable, ushort value)
{
  assignments[variable] = value;
  csp.nassigns++;
}

void unassign(ushort variable)
{
  assignments[variable] = 0;
}

ushort nconflicts(ushort variable, ushort value)
{
  ushort neighbor, n_idx, conflicts = 0;
  for (n_idx = 0; n_idx < NUM_NEIGHBORS; n_idx++) {
    neighbor = neighbors[variable][n_idx];
    if (constraint(variable, value, neighbor, assignments[n_idx]) ==0)
      conflicts++;
  }
  return conflicts;
}

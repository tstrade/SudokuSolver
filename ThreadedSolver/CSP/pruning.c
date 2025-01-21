#include "CSP.h"

/* ------------------------------------------------------------------------
     support_pruning() - Initializes the domain of each
 */

status support_pruning()
{
  ushort slot, val;
  for (slot = 0; slot < NUM_SLOTS; slot++)
    for (val = 0; val < NUM_VALUES; val++)
      csp.domains[slot][val] = (val + 1);

  return SUCCESS;
}

void prune(ushort variable, ushort value)
{
  csp.domains[variable][value] = 0;
}

void infer_assignment()
{
  ushort var, variable, val, value;
  for (var = 0; var < csp.nvars; var++) {
    variable = csp.variables[var];

    if (count(csp.domains[variable], NUM_VALUES) == 1) {
      for (val = 0; val < NUM_VALUES; val++) {

        value = csp.domains[variable][val];
        if (value != 0) {
          assignments[variable] = value;
          break;
        }
      }
    }
  }
}

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

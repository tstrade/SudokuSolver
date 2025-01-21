#include "CSP.h"

void support_pruning()
{
    for (uint8_t var = 0; var < NUM_SLOTS; var++) {
        for (uint8_t val = 1; val <= NUM_VALUES; val++) 
            csp.domains[var][val - 1] = val;
    }
}

void prune(uint8_t variable, uint8_t value)
{
    csp.domains[variable][value] = 0;
}

void infer_assignment()
{
    for (uint8_t i = 0; i < csp.nvars; i++) {
        uint8_t var = csp.variables[i];
        if (count(csp.domains[var], NUM_VALUES) == 1) {
            for (uint8_t val = 0; val < NUM_VALUES; val++) {
                if (csp.domains[var][val] != 0)
                    assignments[var] = csp.domains[var][val];
            }
        } else {
            assignments[var] = 0;
        }
    }
}


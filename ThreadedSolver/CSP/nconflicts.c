#include "CSP.h"

uint8_t nconflicts(uint8_t variable, uint8_t value)
{
    uint8_t conflicts = 0;
    for (uint8_t n_idx = 0; n_idx < NUM_NEIGHBORS; n_idx++) {
        uint8_t nbr = neighbors[variable][n_idx];
        if (constraint(variable, value, nbr, assignments[nbr]) == 0)
            conflicts++;
    }
    return conflicts;
}

uint8_t count(uint8_t *seq, uint8_t size)
{
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < size; i++) {
        if (seq[i] != UNASSIGNED)
            cnt++;
    }
    return cnt;
}

status is_var(uint8_t variable)
{
    for (uint8_t slot = 0; slot < csp.nvars; slot++) {
        if (csp.variables[slot] == variable)
            return ISVAR;
    }
    return NTVAR;
}

uint8_t neighbor_idx(uint8_t slot, uint8_t nbr)
{
    for (uint8_t n_idx = 0; n_idx < NUM_NEIGHBORS; n_idx++) {
        if (neighbors[slot][n_idx] == nbr)
            return n_idx;
    }
    return (uint8_t)FAILURE;
}

status is_neighbor(uint8_t slot, uint8_t nbr)
{
    uint8_t n_idx = neighbor_idx(slot, nbr);
    if (n_idx == (uint8_t)FAILURE)
        return FAILURE;
    if (neighbors[slot][n_idx] == nbr)
        return SUCCESS;
    return FAILURE;
}
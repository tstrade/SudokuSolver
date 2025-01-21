#include "CSP.h"

uint8_t neighbors[NUM_SLOTS][NUM_NEIGHBORS];
uint8_t assignments[NUM_SLOTS];

static status isneighbor(uint8_t slot, uint8_t nbr);

status binit(char *initialPositions)
{
  csp.nassigns = 0;
  csp.nvars = 0;
  
  for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
    if (initialPositions[slot] > ASCII_0 && initialPositions[slot] <= ASCII_9) {
      assign(slot, (uint8_t)initialPositions[slot] - 48);
    } else {
      assignments[slot] = 0;
      csp.nvars++;
    }
  }

  display();

  csp.variables = malloc(csp.nvars * sizeof(int));
  if (csp.variables == NULL)
    return FAILURE;

  // Track all of the current variables on the board
  uint8_t var = 0;
  for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
    if (assignments[slot] == 0) {
      csp.variables[var] = slot;
      var++;
    }
  }

  // Initialize the domains of all the slots
  support_pruning();

  for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
    if (assignments[slot] == 0)
      continue;

    for (uint8_t val = 1; val <= NUM_VALUES; val++) {
      if (val != assignments[slot])
        csp.domains[slot][val - 1] = 0;
    }
  }

  return SUCCESS;
}

status ninit()
{
  uint8_t slot, val, nbr;
  uint8_t row, col, box_start_row, box_start_col;
  uint8_t n_idx, r_idx, c_idx;

  for (slot = 0; slot < NUM_SLOTS; slot++) {
    row = ROW(slot);
    col = COL(slot);

    box_start_row = (row / NUM_BOX) * NUM_BOX;
    box_start_col = (col / NUM_BOX) * NUM_BOX;

    n_idx = 0;

    // Add row neighbors
    for (val = 0; val < NUM_VALUES; val++) {
      nbr = row * NUM_VALUES + val;
      if (nbr != slot && !isneighbor(slot, nbr)) {
        neighbors[slot][n_idx] = nbr;
        n_idx++;
      }
    }

    // Add column neighbors
    for (val = 0; val < NUM_VALUES; val++) {
      nbr = val * NUM_VALUES + col;
      if (nbr != slot && !isneighbor(slot, nbr)) {
        neighbors[slot][n_idx] = nbr;
        n_idx++;
      }
    }

    // Add box neighbors
    for (r_idx = box_start_row; r_idx < box_start_row + NUM_BOX; r_idx++) {
      for (c_idx = box_start_col; c_idx < box_start_col + NUM_BOX; c_idx++) {
        nbr = r_idx * NUM_VALUES + c_idx;
        if (nbr != slot && !isneighbor(slot, nbr)) {
          neighbors[slot][n_idx] = nbr;
          n_idx++;
        }
      }
    }
  }

  return SUCCESS;
}

static status isneighbor(uint8_t slot, uint8_t nbr)
{
  for (uint8_t n = 0; n < NUM_NEIGHBORS; n++) {
    if (neighbors[slot][n] == nbr)
      return ISVAR;
  }
  return NTVAR;
}

void bfree()
{
  free(csp.variables);
  csp.variables = NULL;
}
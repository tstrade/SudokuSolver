#include "CSP.h"

static status isneighbor(ushort slot, ushort nbr);

status ninit()
{
  int slot, val, nbr;
  int row, col, box_start_row, box_start_col;
  int nIndex, r_idx, c_idx;

  for (slot = 0; slot < NUM_SLOTS; slot++) {
    row = ROW(slot);
    col = COL(slot);

    box_start_row = (row / NUM_BOX) * NUM_BOX;
    box_start_col = (col / NUM_BOX) * NUM_BOX;

    nIndex = 0;

    // Add row neighbors
    for (val = 0; val < NUM_VALUES; val++) {
      nbr = row * NUM_VALUES + val;
      if (nbr != slot && !isneighbor(slot, nbr)) {
        neighbors[slot][nIndex] = nbr;
        nIndex++;
      }
    }

    // Add column neighbors
    for (val = 0; val < NUM_VALUES; val++) {
      nbr = val * NUM_VALUES + col;
      if (nbr != slot && !isneighbor(slot, nbr)) {
        neighbors[slot][nIndex] = nbr;
        nIndex++;
      }
    }

    // Add box neighbors
    for (r_idx = box_start_row; r_idx < box_start_row + NUM_BOX; r_idx++) {
      for (c_idx = box_start_col; c_idx < box_start_col + NUM_BOX; c_idx++) {
        nbr = r_idx * NUM_VALUES + c_idx;
        if (nbr != slot && !isneighbor(slot, nbr)) {
          neighbors[slot][nIndex] = nbr;
          nIndex++;
        }
      }
    }
  }

  return SUCCESS;
}

static status isneighbor(ushort slot, ushort nbr)
{
  int n;
  for (n = 0; n < NUM_NEIGHBORS; n++)
    if (neighbors[slot][n] == nbr)
      return ISVAR;

  return NTVAR;
}

#include "CSP_Bits.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct CSP {
  ushort *variables;
  ushort **neighbors;
  ushort *domains;
  ushort *assignments;
};

void initNeighbors(CSP *csp) {
  ushort slot, value, row, col, box_start_row, box_start_col, neighbor, r, c;
  ushort *added, added_idx;

  for (slot = 0; slot < NUM_SLOTS; slot++) {
      added = calloc(NUM_SLOTS, sizeof(ushort));

      row = getRow(slot);
      col = getCol(slot);

      box_start_row = (row / NUM_BOX) * NUM_BOX;
      box_start_col = (col / NUM_BOX) * NUM_BOX;

      neighbor = 0;

      // Add row neighbors
        for (value = 0; value < NUM_VALUES; value++) {
            added_idx = row * NUM_VALUES + value;
            if (added_idx != slot && !added[added_idx]) {
	        csp->neighbors[slot][neighbor] = added_idx;
                added[added_idx] = 1;
                neighbor++;
            }
        }

      // Add column neighbors
        for (value = 0; value < NUM_VALUES; value++) {
            added_idx = slot * NUM_VALUES + col;
            if (added_idx != slot && !added[added_idx]) {
	        csp->neighbors[slot][neighbor] = added_idx;
                added[added_idx] = 1;
                neighbor++;
            }
        }

      // Add box neighbors
        for (r = box_start_row; r < box_start_row + NUM_BOX; r++) {
            for (c = box_start_col; c < box_start_col + NUM_BOX; c++) {
                added_idx = r * NUM_VALUES + c;
                if (added_idx != slot && !added[added_idx]) {
		    csp->neighbors[slot][neighbor] = added_idx;
                    added[added_idx] = 1;
                    neighbor++;
                }
            }
        }

        free(added);
        added = NULL;
    }
}

CSP *initCSP() {
  CSP *csp = malloc(sizeof(CSP));

  csp->variables = calloc(NUM_SLOTS, sizeof(ushort));

  csp->neighbors = malloc(NUM_SLOTS * sizeof(ushort *));
  for (ushort n = 0; n < NUM_SLOTS; n++) {
    csp->neighbors[n] = calloc(NUM_NEIGHBORS, sizeof(ushort));
  }
  initNeighbors(csp);

  csp->domains = calloc(NUM_SLOTS, sizeof(ushort));
  for (ushort slot = 0; slot < NUM_SLOTS; slot++) {
    csp->domains[slot] = 0x01FF;
  }

  csp->assignments = calloc(NUM_SLOTS, sizeof(ushort));

  return csp;
}

ushort nconflicts(CSP *csp, ushort slot, ushort value) {
  ushort neighbor, conflicts = 0;
  for (ushort n = 0; n < NUM_NEIGHBORS; n++) {
    neighbor = csp->neighbors[slot][n];
    if (!constraint(slot, value, neighbor, csp->assignments[neighbor])) { conflicts++; }
  }
  return conflicts;
}

void display(CSP *self) {
  char *assignment = malloc(NUM_SLOTS * sizeof(char));

  for (ushort slot = 0; slot < NUM_SLOTS; slot++) {
    assignment[slot] = self->assignments[slot] + 48;
  }

  printf("\n\n   ---------------------------------\n");

  for (ushort slot = 0; slot < NUM_SLOTS; slot += 9) {
    printf("  | %c  %c  %c  |  %c  %c  %c  |  %c  %c  %c |\n",
    assignment[slot], assignment[slot + 1], assignment[slot + 2],
    assignment[slot + 3], assignment[slot + 4], assignment[slot + 5],
    assignment[slot + 6], assignment[slot + 7], assignment[slot + 8]);

    if (slot == NUM_VALUES * 2 || slot == NUM_VALUES * 5) {
      // Only need two dividers (board is in thirds)
      printf("   ----------+-----------+----------\n");
    }
  }

  printf("   ---------------------------------\n\n");

  free(assignment);
  assignment = NULL;
}

ushort goal_test(CSP *csp) {
  if (countEntries(csp->assignments, NUM_SLOTS) != NUM_SLOTS) { return 0; }
  for (ushort slot = 0; slot < NUM_SLOTS; slot++) {
    if (nconflicts(csp, slot, csp->assignments[slot]) != 0) { return 0; }
  }
  return 1;
}

void prune(CSP *csp, ushort slot, ushort value) {
  csp->domains[slot] &= (~value);
}

void infer_assignment(CSP *csp) {
  ushort var, variable;
  for (var = 1; var < csp->variables[0]; var++) {
    variable = csp->variables[var];
    if (countBits(csp->domains[variable]) == 1) {
      csp->assignments[variable] = getlog(csp->domains[variable] + 1);
    }
    else { csp->assignments[variable] = 0; }
  }
}

ushort constraint(ushort A_slot, ushort A_value, ushort B_slot, ushort B_value) {
  return (((A_slot ^ B_slot) ^ (A_value ^ B_value)) ? 0 : 1);
}

ushort countEntries(ushort *sequence, ushort size) {
  ushort count = 0, slot = 0;
  while (slot < size) { if (!sequence[slot]) { count++; } slot++; }
  return count;
}

ushort countBits(ushort sequence) {
  ushort count = 0;
  while (sequence) { count += (sequence & 1); sequence >>= 1; }
  return count;
}

ushort getRow(ushort slot) {
  return slot / NUM_VALUES;
}

ushort getCol(ushort slot) {
  return slot % NUM_VALUES;
}

ushort isVariable(CSP *csp, ushort slot) {
  if (!csp->assignments[slot]) { return 1; }
  return 0;
}

ushort getlog(ushort n) {
  float f = (float)n;
  unsigned int *p = (unsigned int *)&f;
  return ((LOG_2 & *p) >> SHIFT_EXP) - BIAS;
}

void destroyCSP(CSP *csp) {
  free(csp->variables);
  free(csp->domains);
  free(csp->assignments);
  for (ushort slot = 0; slot < NUM_SLOTS; slot++) { free(csp->neighbors[slot]); }
  free(csp->neighbors);
  free(csp);
  csp = NULL;
}

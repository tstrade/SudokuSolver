#include "CSP.h"

uint8_t goal_test() {
  uint8_t ncnflct;

  // If all variables haven't been assigned, the game is not complete
  if (csp.nassigns != NUM_SLOTS) { 
    printf("Not all slots are assigned!\n"); 
    return 0;
  }
  // If any (Variable, Value) pair has a conflict, the game is not complete
  for (uint8_t var = 0; var < NUM_SLOTS; var++) {
    if ((ncnflct = nconflicts(var, assignments[var])) != 0) {
      printf("Conflict with (%d, %d)!\n", var, assignments[var]); 
      return 0;
    }
    printf("There are %d conflicts with variable %d\n", ncnflct, var);
  }
  printf("The game is complete with %d assignments!\n", csp.nassigns);
  return SUCCESS;
}

status support_pruning() {
  if (csp.domains != NULL) { return; }

  csp.domains = malloc(NUM_SLOTS * sizeof(uint8_t *));
  checkNULL((void *)csp.domains);

  uint8_t var, val;
  for (var = 0; var < NUM_SLOTS; var++) {
    csp.domains[var] = calloc(NUM_VALUES, sizeof(uint8_t));
    checkNULL((void *)csp.domains[var]);

    // Establish base domain values
    for (val = 0; val < NUM_VALUES; val++) {
      csp.domains[var][val] = val + 1;
    }
  }
}


void suppose(CSP *self, uint8_t variable, uint8_t value) {
  // Setup curr_domains if necessary
  support_pruning(self);

  uint8_t val, checkValue;
  for (val = 0; val < NUM_VALUES; val++) {
    checkValue = csp.domains[variable][val];
    if (checkValue != value) {
      // Remove any values that do not align with the supposition
      self->removals[variable][val] = checkValue;
      csp.domains[variable][val] = 0;
    } else {
      // Make sure supposed value is not set in removed "dict"
      self->removals[variable][val] = 0;
      csp.domains[variable][val] = value; // Redundant, but for consistency's sake
    }
  } // End values loop
}

void prune(CSP *self, uint8_t variable, uint8_t value) {
  csp.domains[variable][value - 1] = 0;
  self->removals[variable][value - 1] = value;
}

void infer_assignment(CSP *self) {
  // Setup curr_domains if necessary
  support_pruning(self);

  uint8_t i, var, val;
  for (i = 0; i < self->numVars; i++) {
    var = self->variables[i];

    if (count(csp.domains[var], NUM_VALUES) == 1) {
      for (val = 0; val < NUM_VALUES; val++) {
	if (csp.domains[var][val] != 0) {
	  self->assignment[var] = csp.domains[var][val];
	  break;
	}
      } // End value loop
    } // End check domain size bock
    else {
      self->assignment[var] = 0;
    }
  }
}



uint8_t count(uint8_t *seq, uint8_t size) {
  uint8_t count = 0;
  for (uint8_t i = 0; i < size; i++) {
    if (seq[i] != 0) { count++; }
  } // End values loop
  return count;
}

uint8_t isVariable(CSP *csp, uint8_t variable) {
  uint8_t slot;
  for (slot = 0; slot < csp->numVars; slot++) {
    if (csp->variables[slot] == variable) { return 1; }
  }
  return 0;
}

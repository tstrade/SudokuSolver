#ifndef _CSP_H
#define _CSP_H

typedef struct CSP CSP;

// Construct the constraint satisfaction problem
CSP *__init__();
// Add the variable with its value to the assignment (discarding old value, if any)
//     "assignment" is a list of ints to represent each space on a 9x9 Soduku board (index 0-80)
void assign(CSP *self, int variable, int value, int *assignment);
// Remove variable from assignment (DO NOT use for reassigning a variable's value)
void unassign(CSP *self, int variable, int *assignment);
// Return number of conflicts variable = value has with other variables
int nconflicts(CSP *self, int variable, int value, int *assignment);
// Show human-readable representation of CSP
void display(CSP *self, int *assignment);

// ************Following methods are for tree / graph traversal************** //

// Return list of applicable actions (nonconfliction assignments to an unassigned variable)
int **actions(CSP *self, int *state);
// Perform action and return new state
int *result(CSP *self, int *state, int *action);
// Check if Soduku is solved correctly
int goal_test(CSP *self, int *state);
// Want to be able to prune values from domains
void support_pruning(CSP *self);
// Accumulate inferences from assuming variable = value
int **suppose(CSP *self, int variable, int value);
// Rule out variable = value
void prune(CSP *self, int variable, int value, int **removals);
// Undo supposition and all inferences from it
void restore (CSP *self, int **removals);

// ************Other************** //

// Apply rules of Soduku
int Soduku_Constraint(int varA, int valA, int varB, int valB);
// Return number of true instances
int count(int **seq);

int getRow(int variable);
int getCol(int variable);

#endif

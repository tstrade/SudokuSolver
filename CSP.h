#ifndef CSP_H
#define CSP_H

typedef struct CSP CSP;


CSP *initCSP(CSP *csp);
int sortNeighbors(const void *a, const void *b);
void checkNULL(void *ptr);
void initNeighbors(CSP *csp);
void assign(CSP *self, int variable, int value);
void unassign(CSP *self, int variable);
int nconflicts(CSP *self, int variable, int value);
void display(CSP *self);
int **actions(CSP *self);
int *result(CSP *self, int *action);
void support_pruning(CSP *self);
void suppose(CSP *self, int variable, int value);
void prune(CSP *self, int variable, int value);
void infer_assignment(CSP *self);
void restore(CSP *self);
int Soduku_Constraint(int varA, int valA, int varB, int valB);
int count(int *seq);
int getRow(int variable);
int getCol(int variable);
void destroyCSP(CSP *self);

#endif

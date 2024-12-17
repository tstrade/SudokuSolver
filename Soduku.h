#include "CSP.h"

typedef struct Soduku Soduku;

Soduku *initBoard(CSP *csp, char *initialPositions);
void showVars(Soduku *self);
void showCurrentDomains(Soduku *self);
void showVariableDomain(Soduku *self, int variable);
void showRemovals(Soduku *self);
void destroySoduku(Soduku *self);

#ifndef SODUKU_H
#define SODUKU_H

typedef struct Soduku Soduku;

Soduku *initBoard(Soduku **self, char *initialPositions);
void showVars(Soduku *self);
void showCurrentDomains(Soduku *self);
void showVariableDomain(Soduku *self, int variable);
void showRemovals(Soduku *self);
void destroySoduku(Soduku **self);

#endif

#ifndef THREADED_SODUKU_H
#define THREADED_SODUKU_H

#include "CSP.h"

CSP *initBoard(CSP *board, char *initialPositions);
void showVars(CSP *board);
void showCurrentDomains(CSP *board);
void showVariableDomain(CSP *board, int variable);
void showRemovals(CSP *board);
void destroySoduku(CSP *board);

#endif

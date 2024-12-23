#ifndef SODUKU_H
#define SODUKU_H

#include "CSP_Bits.h"

CSP *initBoard(char *initialPositions);
void showVars(CSP *board);
void showCurrentDomains(CSP *board);
void showVariableDomain(CSP *board, int variable);
void destroySoduku(CSP *board);

#endif

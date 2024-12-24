#ifndef SODUKU_H
#define SODUKU_H

CSP *initBoard(CSP *board, char *initialPositions);
void showVars(CSP *board);
void showCurrentDomains(CSP *board);
void showVariableDomain(CSP *board, int variable);
void showRemovals(CSP *board);
void destroySoduku(CSP *board);

#endif

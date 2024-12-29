#ifndef AC3_STRUCTS_H
#define AC3_STRUCTS_H

#include <pthread.h>
#include "CSP.h"
#include "queue.h"

typedef struct AC3 AC3;
typedef struct Revisors Revisors;
typedef struct HOA HOA;

AC3 *initAC3(CSP *csp, Queue *q, pthread_t *parent, pthread_t *finish);
Revisors *initRevisors(AC3 *top_level);
HOA *initHOA(AC3 *top_level);
void destroyAC3(AC3 *solver);
void destroyRevisors(Revisors *worker);
void destroyHOA(HOA *inspector);

#endif

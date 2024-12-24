#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include "CSP.h"
#include "datastructs.h"

typedef struct AC3_args AC3_args;
typedef struct protector protector;

AC3_args *initAC3Args(CSP *csp, protector *knight);
protector *initProtector();

void destroyAC3Args(AC3_args *args);
void destroyProtector(protector *knight);

#endif

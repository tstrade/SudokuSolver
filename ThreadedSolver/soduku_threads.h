#ifndef SODUKU_THREADS_H
#define SODUKU_THREADS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/* ----- Types ----- */
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef int32_t status;


/* ----- Macros ----- */
#define SUCCESS 1
#define FAILURE 0

#define NUM_VALUES 9
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#define NUM_NEIGHBORS ((NUM_VALUES - 1) + (2 * (NUM_VALUES - (NUM_VALUES/3))))
#define NUM_BOX (NUM_VALUES / 3)


/* ----- Inline Functions ----- */
#define EXIT exit(EXIT_FAILURE);

typedef struct AC3 AC3;
typedef struct Revisors Revisors;
typedef struct HOA HOA;


/* ----- Initializing Structures Functions ----- */

int initQueue();

int initAC3(pthread_t *parent, pthread_t *finish);
int initRevisors();
int initHOA();


/* ----- Terminating Mutexes, etc. ----- */
void destroyAC3(AC3 *solver);
void destroyRevisors(Revisors *worker);
void destroyHOA(HOA *inspector);


/* ----- Payload Functions to Execute the AC3 Algorithm ----- */
void *editQueue(void *arg);
void *scanQueue(void *arg);
void *processTuple(void *arg);
void *revise(void *arg);
void *inspect(void *arg);


void destroySoduku();


#endif

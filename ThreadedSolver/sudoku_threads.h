#ifndef SODUKU_THREADS_H
#define SODUKU_THREADS_H

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/* ----- Types ----- */
typedef int32_t status;

/* ----- Macros ----- */
#define SUCCESS  0
#define FAILURE  (-1)

#define TRUE     1
#define FALSE    0

#define NUM_VALUES 9
#define NUM_SLOTS (NUM_VALUES * NUM_VALUES)
#define NUM_NEIGHBORS ((NUM_VALUES - 1) + (2 * (NUM_VALUES - (NUM_VALUES/3))))
#define NUM_BOX (NUM_VALUES / 3)

#define ASCII_9 57
#define ASCII_0 48

/* ----- Inline Functions ----- */
#define EXIT (exit(EXIT_FAILURE))

#endif

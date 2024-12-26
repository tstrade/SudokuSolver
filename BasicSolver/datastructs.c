#include "datastructs.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef NUM_SLOTS
#define NUM_SLOTS 81
#endif

#ifndef NUM_NEIGHBORS
#define NUM_NEIGHBORS 20
#endif

#define INIT_SIZE NUM_SLOTS * NUM_NEIGHBORS

struct Queue {
  int **tuples;
  int **isInQueue;
  int maxSize;
  int currSize;
  int head;
  int tail;
};

Queue *initQueue(Queue *q) {
  q = malloc(sizeof(Queue));
  if (q == NULL) {
    fprintf(stderr, "Malloc failed on Queue struct!\n");
    exit(EXIT_FAILURE);
  }

  q->maxSize = INIT_SIZE;
  q->currSize = 0;
  q->head = 0;
  q->tail = 0;

  q->tuples = malloc(INIT_SIZE * sizeof(int *));
  if (q->tuples == NULL) {
    fprintf(stderr, "Malloc failed on tuples list!\n");
    exit(EXIT_FAILURE);
  }

  int tupleIndex;
  for (tupleIndex = 0; tupleIndex < INIT_SIZE; tupleIndex++) {
    q->tuples[tupleIndex] = calloc(2, sizeof(int));
    if (q->tuples[tupleIndex] == NULL) {
      fprintf(stderr, "Calloc failed on tuple #%d!\n", tupleIndex);
      exit(EXIT_FAILURE);
    }
  }

  q->isInQueue = malloc(NUM_SLOTS * sizeof(int *));
  if (q->isInQueue == NULL) {
    fprintf(stderr, "Malloc failed on isInQueue!\n");
    exit(EXIT_FAILURE);
  }

  int inQIndex;
  for (inQIndex = 0; inQIndex < NUM_SLOTS; inQIndex++) {
    q->isInQueue[inQIndex] = calloc(NUM_SLOTS, sizeof(int));
    if (q->isInQueue[inQIndex] == NULL) {
      fprintf(stderr, "Calloc failed on isInQueue #%d!\n", inQIndex);
      exit(EXIT_FAILURE);
    }
  }

  return q;
}

void enqueue(Queue *q, int Xi, int Xj) {
  q->isInQueue[Xi][Xj] = 1;
  q->currSize += 1;

  q->tuples[q->tail][0] = Xi;
  q->tuples[q->tail][1] = Xj;
  q->tail = (q->tail + 1) % q->maxSize;

}

void dequeue(Queue *q, int *item) {
  item[0] = q->tuples[q->head][0];
  item[1] = q->tuples[q->head][1];

  q->isInQueue[item[0]][item[1]] = 0;

  q->head = (q->head + 1) % q->maxSize;
  q->currSize -= 1;
}

int *peek(Queue *q) {
  return q->tuples[q->head];
}

int isFull(Queue *q) {
  return (q->currSize >= q->maxSize);
}

int isEmpty(Queue *q) {
  return (q->currSize == 0);
}

void destroyQueue(Queue *q) {
  int tuplesIndex;
  for (tuplesIndex = 0; tuplesIndex < q->maxSize; tuplesIndex++) {
    free(q->tuples[tuplesIndex]);
    q->tuples[tuplesIndex] = NULL;
  }

  int inQIndex;
  for (inQIndex = 0; inQIndex < NUM_SLOTS; inQIndex++) {
    free(q->isInQueue[inQIndex]);
    q->isInQueue[inQIndex] = NULL;
  }

  free(q->tuples);
  free(q->isInQueue);
  free(q);
  q = NULL;
}

#include "datastructs.h"
#include <stdlib.h>
#include <stdio.h>

#define INIT_SIZE 100
#define THRESHOLD 0.85

struct Queue {
  int **tuples;
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

  return q;
}

void enqueue(Queue *q, int Xi, int Xj) {
  q->currSize += 1;
  if (isFull(q)) { resizeQueue(q); }

  //printf("Endqueuing (Variable %d, Neighbor %d)\n", Xi, Xj);
  q->tuples[q->tail][0] = Xi;
  q->tuples[q->tail][1] = Xj;
  q->tail = (q->tail + 1) % q->maxSize;

}

void dequeue(Queue *q, int *item) {
  item[0] = q->tuples[q->head][0];
  item[1] = q->tuples[q->head][1];

  q->head += 1;
  q->currSize -= 1;
}

int *peek(Queue *q) {
  return q->tuples[q->head];
}

int isFull(Queue *q) {
  return (q->currSize >= THRESHOLD * q->maxSize);
}

int isEmpty(Queue *q) {
  return (q->currSize == 0);
}

void resizeQueue(Queue *q) {
  int newSize = q->maxSize * 2;
  q->tuples = realloc(q->tuples, newSize * sizeof(int *));
  if (q->tuples == NULL) {
    fprintf(stderr, "Realloc failed on resizing!\n");
    exit(EXIT_FAILURE);
  }

  int extend;
  for (extend = q->maxSize; extend < newSize; extend++) {
    q->tuples[extend] = calloc(2, sizeof(int));
    if (q->tuples[extend] == NULL) {
      fprintf(stderr, "Calloc failed on resizing!\n");
      exit(EXIT_FAILURE);
    }
  }

  q->maxSize = newSize;
}

void destroyQueue(Queue *q) {
  int tuplesIndex;
  for (tuplesIndex = 0; tuplesIndex < q->maxSize; tuplesIndex++) {
    free(q->tuples[tuplesIndex]);
    q->tuples[tuplesIndex] = NULL;
  }

  free(q->tuples);
  free(q);
  q = NULL;
}

#include "datastructs.h"
#include <stdlib.h>

struct Queue {
  int currSize;
  int maxSize;
  int **items;
};

Queue *__initQueue__(int initSize) {
  Queue *q = malloc(sizeof(Queue));
  if (q == NULL) { return NULL; } // Not enough memory!

  q->currSize = 0; // These will be used to compute a ratio
  q->maxSize = initSize; // So the queue can be expanded if necessary
  q->items = malloc(initSize * sizeof(void *));
  if (q->items == NULL) { return NULL; }; // Not enough memory!
  for (int i = 0; i < initSize; i++) {
    q->items[i] = malloc( * sizeof(int)); // For Soduku, each variable

  return q;
}

void *enqueue(Queue *q, void *item) {

}

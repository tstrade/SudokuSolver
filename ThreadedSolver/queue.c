#include "queue.h"

#include <stdlib.h>

struct Queue {
  int currSize;
  Tuple *head;
  Tuple *tail;
};


struct Tuple {
  int Xi, Xj;
  Tuple *next;
};

Queue *initQueue() {
  Queue *q = malloc(sizeof(Queue));
  if (q == NULL) { exit(EXIT_FAILURE); }

  q->currSize = 0;
  q->head = malloc(sizeof(Tuple));
  q->tail = q->head;
  return q;
}

void enqueue(Queue *q, int Xi, int Xj) {
  if (q->currSize == 0) {
    q->head->Xi = Xi, q->head->Xj = Xj;
    q->currSize++;
    return;
  }

  Tuple *new = malloc(sizeof(Tuple));
  if (new == NULL) { EXIT; }

  new->Xi = Xi;
  new->Xj = Xj;

  q->tail->next = new;
  q->tail = new;

  q->currSize++;
}

void dequeue(Queue *q, int *tuple) {
  tuple[0] = q->head->Xi;
  tuple[1] = q->head->Xj;

  Tuple *temp = q->head;
  q->head = q->head->next;
  q->currSize--;
  free(temp);
}

void destroyQueue(Queue *q) {
  if (q->head) {
    Tuple *temp;
    while (q->head) {
      temp = q->head;
      q->head = q->head->next;
      free(temp);
    }
  }

  free(q);
  q = NULL;
}

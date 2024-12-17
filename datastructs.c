#include "datastructs.h"
#include <stdlib.h>
#include <stdio.h>

struct Queue {
  int *varValTuple;
  Queue *head;
  Queue *next;
  void (*enqueue)(Queue *q, int *item);
  Queue *(*dequeue)(Queue *q);
  int *(*peek)(Queue *q);
};

Queue *initQueue(Queue *head) {
  Queue *q;
  if ((q = malloc(sizeof(Queue))) == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  if ((q->varValTuple = calloc(2, sizeof(int))) == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  if (head == NULL) {
     q->head = q;
  } else {
    q->head = head;
  }

  return q;
}

void enqueue(Queue *q, int *item) {
  Queue *head = q->head; // Next item should know the current head of queue
  if (q->varValTuple[1] == 0) { // A value is between 1 - 9,
    q->varValTuple = item; // val = 0 means the tuple hasn't been set yet
    return;
  }

  while (q->next != NULL) { // Iterate to the end of the queue
    q = q->next;
  }
  q->next = initQueue(head); // Initialize next item in queue
  q->next->varValTuple = item; // Set tuple value
  q->next->head = head;
}

Queue *dequeue(Queue *q) {
  Queue *newHead = q->next;
  Queue *iterQ = q->next;
  newHead->head = newHead; // Update the front of the queue
  destroyQueue(q);
  while (iterQ->next != NULL) { // Update for the rest of the queue
    iterQ->next->head = newHead;
    iterQ = iterQ->next;
  }
  return newHead;
}

int *peek(Queue *q) {
  if (q != NULL) { return q->varValTuple; }
  else { return NULL; }
}

void destroyQueue(Queue *q) {
  free(q->varValTuple);
  free(q);
}

#ifndef QUEUE_H
#define QUEUE_H

#define EXIT exit(EXIT_FAILURE)

typedef struct Queue Queue;
typedef struct Tuple Tuple;

Queue *initQueue();
void enqueue(Queue *q, int Xi, int Xj);
void dequeue(Queue *q, int *tuple);
void destroyQueue(Queue *q);

#endif

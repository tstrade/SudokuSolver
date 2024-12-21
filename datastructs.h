#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

typedef struct Queue Queue;
Queue *initQueue(Queue **queue);
void enqueue(Queue **q, int Xi, int Xj);
void dequeue(Queue **q, int **item);
int *peek(Queue *q);
int isFull(Queue *q);
int isEmpty(Queue *q);
void resizeQueue(Queue **q);
void destroyQueue(Queue **q);


typedef struct Stack Stack;
Stack *initStack(Stack **s);
void push(Stack **s, int *item);
void pop(Stack **s);
void destroyStack(Stack **s);

#endif

#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H


typedef struct Queue Queue;
Queue *initQueue(Queue *q, Queue *head);
void enqueue(Queue *q, int *item);
Queue *dequeue(Queue *q);
Queue *processQueue(Queue *q);
int *peek(Queue *q);
void destroyQueue(Queue *q);


typedef struct entry entry;
entry *initEntry(unsigned int key, int *values);
typedef struct dict dict;
dict *initDict(int initSize);
unsigned int hash(unsigned int key);
unsigned int unhash(unsigned int hashedKey);
entry *getValue(dict *d, unsigned int hashedKey);
entry *setValue(dict *d, unsigned int hashedKey, int value);
entry *removeValue(dict *d, unsigned int hashedKey, int value);
int setKey(dict *d, unsigned int hashedKey);
int removeKey(dict *d, unsigned int hashedKey);

#endif

#ifndef _DATASTRUCTURES_H
#define _DATASTRUCTURES_H

// Queue will be based off of a wrapped array
// If the queue becomes 90% full, double the size
typedef struct Queue Queue;
Queue *initQueue(Queue *head);
void enqueue(Queue *q, int *item);
Queue *dequeue(Queue *q);
int *peek(Queue *q);
void destroyQueue(Queue *q);

/********************************************************************/

// An entry will be a (unhashed) key and a pointer to a value of any type
typedef struct entry entry;
entry *initEntry(unsigned int key, int *values);
// Dictionary will contain a "hash table"
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

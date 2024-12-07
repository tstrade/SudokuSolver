#ifndef _DATASTRUCTURES_H
#define _DATASTRUCTURES_H

// Queue will be based off of a wrapped array
// If the queue becomes 90% full, double the size
typedef struct Queue Queue;
Queue *__initQueue__(int initSize);
void *enqueue(Queue *q, void *item);
void *dequeue(Queue *q, int index);
void *peek(Queue *q);
int isEmpty(Queue *q);
int isFull(Queue *q);

/********************************************************************/

// An entry will be a (unhashed) key and a pointer to a value of any type
typedef struct entry entry;
entry *__initEntry__(unsigned int key, int *values);
// Dictionary will contain a "hash table"
typedef struct dict dict;
dict *__initDict__(int initSize);
unsigned int hash(unsigned int key);
unsigned int unhash(unsigned int hashedKey);
entry *getValue(dict *d, unsigned int hashedKey);
entry *setValue(dict *d, unsigned int hashedKey, int value);
entry *removeValue(dict *d, unsigned int hashedKey, int value);
int setKey(dict *d, unsigned int hashedKey);
int removeKey(dict *d, unsigned int hashedKey);

#endif

#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#define HASH 0xE3AF1BDC
#define CAPACITY 50000

typedef struct hashed_item hashed_item;
typedef struct HashTable HashTable;

HashTable *initHashTable(int size);
hashed_item *initHashedItem(int key, int *values);
int hash_it_up(int h);
void destroyHashItem(hashed_item *item);
void destroyHashTable(HashTable *ht);
void htInsert(HashTable *ht, int key, int value);




#endif

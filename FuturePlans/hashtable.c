#include "hashtable.h"

#include <stdlib.h>

struct hashed_item {
  int key;
  int *values;
};

struct HashTable {
  hashed_item **items;
  int size;
  int count;
};

HashTable *initHashTable(int size) {
  HashTable *ht = malloc(sizeof(HashTable));
  ht->size = size;
  ht->count = 0;
  ht->items = calloc(ht->size, sizeof(hashed_item));
  for (int i = 0; i < ht->size; i++) {
    ht->items[i] = NULL;
  }

  return ht;
}

hashed_item *initHashedItem(int key, int *values) {
  hashed_item *item = malloc(sizeof(hashed_item));
  item->key = key;
  item->values = values;

  return item;
}

int hash_it_up(int key) {
  key = (~key) + (key << 21);
  key ^= (key >> 24);
  key = (key + (key << 3)) + (key << 8);
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4);
  key = key ^ (key >> 28);
  key = key + (key << 31);
  return key;
}

void destroyHashItem(hashed_item *item) {
  free(item->values);
  free(item);
  item = NULL;
}

void destroyHashTable(HashTable *ht) {
  for (int i = 0; i < ht->size; i++) {
    if (ht->items[i] != NULL) { free(ht->items[i]); }
  }
  free(ht->items);
  free(ht);
  ht = NULL;
}

void htInsert(HashTable *ht, int key, int value) {

}

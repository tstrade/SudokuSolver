// Implementation derived from https://github.com/benhoyt/ht/blob/master/ht.c
// Slightly modified to fit use-case of Soduku Solver

#include "hashtable.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 81
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Hashtable entry. Slots may be filled or empty
typedef struct {
  const int *key;
  int *value;
} entry;

// Hashtable structure
struct hashtab {
  entry *entries;
  size_t capacity;
  size_t length;
};

hashtab *hashtab_create(void) {
  // Allocate space for hashtable
  hashtab *table = malloc(sizeof(hashtab));

  // Return NULL if not enough memory for table structure
  if (table == NULL) { return NULL; }

  // Initialize hashtable if memory is allocated
  table->length = 0;
  table->capacity = INITIAL_CAPACITY;

  // Allocate space for entry buckets
  table->entries = calloc(table->capacity, sizeof(entry));
  // Return NULL if not enough memory for entries
  if (table->entries == NULL) { free(table); return NULL; }

  return table;
}

void hashtab_destroy(hashtab *table) {
  // Free allocated keys
  for (size_t i = 0; i < table->capacity; i++) {
    free((void *)table->entries[i].key);
  }
  // Free the entries array and table
  free(table->entries);
  free(table);
}

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const int *key) {
  uint64_t hash = FNV_OFFSET;
  hash ^= (uint64_t)key;
  hash *= FNV_PRIME;
  return hash;
}

int *hashtab_get(hashtab *table, const int *key) {
  // AND hash with capacitity-1 to ensure it's within entries array
  uint64_t hash = hash_key(key);
  size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

  // Loop until empty entry;
  while (table->entries[index].key != NULL) {
    if (*key == *table->entries[index].key) {
      // Found key - return value (pointer to start of int array)
      return table->entries[index].value;
    }
    // Key wasn't in this slot, move to next (linear probing)
    if (++index >= table->capacity) {
      // At end of entries array, wrap around
      index = 0;
    }
  }
  return NULL;
}

// Internal function to set an entry w/o expanding table
static const int *hashtab_set_entry(entry *entries, size_t capacity, const int *key,
                                    int *value, size_t *plength) {
  // AND hash with capacitity-1 to ensure it's within entries array
  uint64_t hash = hash_key(key);
  size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

  // Loop until we find an empty entry
  while (entries[index].key != NULL) {
    if (*key == *entries[index].key) {
      // Key already exists - update value
      entries[index].value = value;
      return entries[index].key;
    }
    // Key wasn't in this slot, move to next (linear probing)
    if (++index >= capacity) { index = 0; }
  }

  // Didn't find key, allocate + copy if needed, then insert it
  if (plength != NULL) {
    if (key == NULL) { return NULL; }
    (*plength)++;
  }
  entries[index].key = key;
  entries[index].value = value;
  return key;
}

const int *hashtab_set(hashtab *table, const int *key, int **value) {
  assert(value != NULL);
  if (value == NULL) { return NULL; }

  // Set entry and update length
  return hashtab_set_entry(table->entries, table->capacity, key, *value, &table->length);
}

size_t hashtab_length(hashtab *table) {
  return table->length;
}

hti ht_iterator(hashtab *table) {
  hti iterator;
  iterator._table = table;
  iterator._index = 0;
  return iterator;
}

bool hashtab_next(hti *iterator) {
  // Loop until we've hit end of entries array
  hashtab *table = iterator->table;
  while (iterator->_index < table->capacity) {
    size_t i = iterator->_index;
    iterator->_index++;
    if (table->entries[i].key != NULL) {
      // Found next non-empty item, update iterator key and value
      entry ntry = table->entries[i];
      iterator->key = ntry.key;
      iterator->value = &ntry.value;
      return true;
    }
  }
  return false;
}

#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <stdbool.h>
#include <stddef.h>


// Hashtable implementation from https://benhoyt.com/writings/hash-table-in-c/
typedef struct hashtab hashtab;

// Create table and return pointer, or NULL if out of memory
hashtab *hastab_create(void);

 // Free allocated memory, including keys
void hastab_destroy(hashtab *table);

// Get item with given key (int for Soduku Solver) from has table
// Return value (set with hashtab_set) or NULL if key not found
void *hastab_get(hashtab *table, const int *key);

// Set item with given key (int) to value (list of ints - not NULL)
// If already present, key is copied to newly allocated memory
// Keys automaticaly freed by hashtab_destroy
// Return address of copied key, or NULL if out of memory
const int *hashtab_set(hashtab *table, const int *key, int **value);

// Return number of items in hash table
size_t hashtab_length(hashtab *table);

// Hashtable iterator
typedef struct {
  const int *key;
  int **value;

  // Indirect / hidden fields
  hashtab *_table; // reference to hashtable being iterated
  size_t _index; // current index into hashtab._entries
} hti;

// Return new hash table iterator (for use with hashtab_next)
hti ht_iterator(hashtab *table);

// Move iterator to next item in hastable
// Update iterator key and value to current item and return trye
// If no more items, return false
// Don't call hashtab_set during iteration
bool hashtab_next(hti *iterator);

#endif

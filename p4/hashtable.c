#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "hashtable.h"

#define OFFSET 2166136261
#define FNV 16777619

struct hashtable {
    // DONE: define hashtable struct to use linkedlist as buckets
    linkedlist_t** kv_arr;
    int size;
    int num_buckets;
};

/**
 * Hash function to hash a key into the range [0, max_range)
 * Implementation of FNV Hash for 32 bit key.
 */
int hash(int key, int max_range) {
  /*  int selector = 255;
    int hash = OFFSET;
    int i;
    for(i = 0; i < 4; i++) {
        hash = hash ^ ((key & (selector << i * 8)) >> i * 8);
        hash = hash * FNV;
    }
*/  return key % max_range;
}

hashtable_t *ht_init(int num_buckets) {
    // DONE: create a new hashtable
    int i;
    hashtable_t* ht = (hashtable_t*) malloc(sizeof(hashtable_t));
    ht->kv_arr = (linkedlist_t**) malloc(num_buckets * sizeof(linkedlist_t*));
    ht->size = 0;
    ht->num_buckets = num_buckets;
    for (i = 0; i < num_buckets; i++) {
        ht->kv_arr[i] = ll_init();
    }
    return ht;
}

void ht_add(hashtable_t *table, int key, int value) {
    // DONE: create a new mapping from key -> value.
    // If the key already exists, replace the value.
    int oldsize;
    linkedlist_t* hitlist = table->kv_arr[hash(key, table->num_buckets)];
    oldsize = ll_size(hitlist);
    ll_add(hitlist, key, value);
    table->size +=  ll_size(hitlist) - oldsize;
}

int ht_get(hashtable_t *table, int key) {
    // DONE: retrieve the value mapped to the given key.
    // If it does not exist, return 0
    linkedlist_t* hitlist = table->kv_arr[hash(key, table->num_buckets)];
    return ll_get(hitlist, key);
}

int ht_size(hashtable_t *table) {
    // DONE: return the number of mappings in this hashtable
    return table->size;
}

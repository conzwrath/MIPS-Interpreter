#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

struct linkedlist {
    struct linkedlist_node *first;
    // TODO: define linked list metadata
    int size;
};

typedef struct linkedlist_node {
    // TODO: define the linked list node
    int key;
    int val;
    struct linkedlist_node* next;
} linkedlist_node_t;

linkedlist_t *ll_init() {
    // TODO: create a new linked list
    linkedlist_t* a = (linkedlist_t*)malloc(sizeof(linkedlist_t));
    a->first = NULL;
    a->size = 0;
    return a;
}

/*
 * Add node to front of list regardless of key value
 */
void ll_prepend(linkedlist_t *list, int key, int value) {
       linkedlist_node_t* newstuff = (linkedlist_node_t*)malloc(sizeof(linkedlist_node_t));
       newstuff->key = key;
       newstuff->val = value;
       newstuff->next = list->first;
       list->first = newstuff;
       list->size++;
}

void ll_add(linkedlist_t *list, int key, int value) {
    // TODO: create a new node and add to the front of the linked list if a
    // node with the key does not already exist.
    // Otherwise, replace the existing value with the new value.
    linkedlist_node_t* current = list->first;
    int i;
    for (i = 0; i < list->size; i++) { 	    
        if (current->key == key) {
            current->val = value;
            return;
	    }
        current = current->next;
    }
    ll_prepend(list, key, value);
}

int ll_get(linkedlist_t *list, int key) {
    // TODO: go through each node in the linked list and return the value of
    // the node with a matching key.
    // If it does not exist, return 0.
    linkedlist_node_t* current = list->first;
    int i;
    for (i = 0; i < list->size; i++) {
        if(current->key == key) {
            return current->val;
        }
        current = current->next;
    }
    return 0;
}

int ll_size(linkedlist_t *list) {
    // TODO: return the number of nodes in this linked list
    return list->size;
}

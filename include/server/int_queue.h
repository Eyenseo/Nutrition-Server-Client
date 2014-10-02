#ifndef INT_QUEUE_H
#define INT_QUEUE_H

#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node node_t;

struct node {
  int data;
  node_t* next;
};

typedef struct {
  node_t* first;
  node_t* last;
  pthread_mutex_t mutex;
} int_queue_t;

bool int_queue_create(int_queue_t** const q);
void int_queue_destroy(int_queue_t* const q);

bool int_queue_push_back(int_queue_t* const q, int data);
bool int_queue_pop(int_queue_t* const q, int* const data);

#endif

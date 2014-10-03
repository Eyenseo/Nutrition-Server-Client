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

/**
 * @param  q pointer to store the queue to
 *
 * @return   true if successful
 */
bool int_queue_create(int_queue_t** const q);

/**
 * @param q queue to be destroyed
 */
void int_queue_destroy(int_queue_t* const q);

/**
 * THe function appends the data value to the queue in a thread save way
 *
 * @param  q    queue to append the data to
 * @param  data  data to append
 *
 * @return      true if successful
 */
bool int_queue_push_back(int_queue_t* const q, int data);

/**
 * The function returns and deletes the first element of the queue in a thread
 * save way
 *
 * @param  q    queue to get the first element from
 * @param  data pointer to store the data to
 *
 * @return      true if successful
 */
bool int_queue_pop(int_queue_t* const q, int* const data);

#endif

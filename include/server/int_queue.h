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

bool int_queue_create(int_queue_t** const q) {
  if(q != NULL) {
    *q = malloc(sizeof(int_queue_t));

    (*q)->first = NULL;
    (*q)->last = NULL;
    pthread_mutex_init(&(*q)->mutex, NULL);

    return true;
  }
  return false;
}

void int_queue_destroy(int_queue_t* const q) {
  if(q != NULL) {
    pthread_mutex_lock(&q->mutex);

    node_t* n = q->first;
    node_t* c = n;

    while(n != NULL) {
      c = n;
      n = n->next;
      free(c);
    }

    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);

    free(q);
  }
}

bool int_queue_push_back(int_queue_t* const q, int data) {
  if(q != NULL) {
    pthread_mutex_lock(&q->mutex);
    node_t* n = malloc(sizeof(node_t));
    n->data = data;
    n->next = NULL;

    if(q->first == NULL) {
      q->first = n;
    } else {
      q->last->next = n;
    }
    q->last = n;

    pthread_mutex_unlock(&q->mutex);
    return true;
  }
  return false;
}

bool int_queue_pop(int_queue_t* const q, int* const data) {
  if(q != NULL && data != NULL) {
    pthread_mutex_lock(&q->mutex);

    node_t* n = q->first;
    if(n != NULL) {
      q->first = n->next;
      *data = n->data;
      free(n);
    } else {
      pthread_mutex_unlock(&q->mutex);
      return false;
    }

    pthread_mutex_unlock(&q->mutex);
    return true;
  }
  return false;
}


#endif

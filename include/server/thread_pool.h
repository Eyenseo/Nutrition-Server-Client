#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>

#define THREAD_POOL_SIZE 10

typedef struct {
  pthread_t pool[THREAD_POOL_SIZE];
  sem_t sem;
  void (*fun)(int, void*);
  void* arg;
  bool running;
} thread_pool_t;

typedef struct {
  int id;
  thread_pool_t* tp;
} thread_pool_param_t;

bool thread_pool_create(thread_pool_t** const tp);
bool thread_pool_destroy(thread_pool_t* const tp);

bool thread_pool_start(thread_pool_t* const tp, void (*fun)(int, void*),
                       void* arg);
bool thread_pool_stop(thread_pool_t* const tp);
bool thread_pool_notify(thread_pool_t* const tp);

void thread_pool_work_wrapper(thread_pool_param_t* const p);

#endif

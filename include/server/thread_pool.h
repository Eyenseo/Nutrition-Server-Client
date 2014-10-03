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

/**
 * @param  tp pointer variable to store the thread pool to
 *
 * @return    true if successful
 */
bool thread_pool_create(thread_pool_t** const tp);

/**
 * @param  tp thread pool to be destroyed
 *
 * @return    true if successful
 */
bool thread_pool_destroy(thread_pool_t* const tp);

/**
 * The function starts a non started thread pool
 *
 * @param  tp  thread pool to be started
 * @param  fun function to be executed by the thread pool
 * @param  arg argument to pass to the threads
 *
 * @return     true if successful
 */
bool thread_pool_start(thread_pool_t* const tp, void (*fun)(int, void*),
                       void* arg);

/**
 * The function stops a running thread pool
 *
 * @param  tp thread pool to stop
 *
 * @return    true if successful
 */
bool thread_pool_stop(thread_pool_t* const tp);

/**
 * THe function notifies the thread pool that there is additional work
 *
 * @param  tp thread pool to be notified
 *
 * @return    true if successful
 */
bool thread_pool_notify(thread_pool_t* const tp);

/**
 * Wrapper function for the thread pool - don't execute
 *
 * @param p parameter stuct
 */
void thread_pool_work_wrapper(thread_pool_param_t* const p);

#endif

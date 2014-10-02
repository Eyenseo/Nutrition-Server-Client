#include "server/thread_pool.h"

#include <stdio.h>

void thread_pool_work_wrapper(thread_pool_param_t* const p) {
  while(p->tp->running) {
    sem_wait(&p->tp->sem);

    if(p->tp->running) {
      p->tp->fun(p->id, p->tp->arg);
    }
  }
  thread_pool_notify(p->tp);
  thread_pool_notify(p->tp);

  free(p);
}

bool thread_pool_stop(thread_pool_t* const tp) {
  if(tp != NULL && tp->running) {
    tp->running = false;
    thread_pool_notify(tp);

    for(int i = 0; i < THREAD_POOL_SIZE; ++i) {
      pthread_join(tp->pool[i], NULL);
    }
  }
  return false;
}

bool thread_pool_create(thread_pool_t** const tp) {
  if(tp != NULL) {
    *tp = malloc(sizeof(thread_pool_t));

    sem_init(&(*tp)->sem, 0, 0);
    (*tp)->running = false;

    return true;
  }
  return false;
}

bool thread_pool_destroy(thread_pool_t* const tp) {
  if(tp != NULL) {
    thread_pool_stop(tp);

    sem_destroy(&tp->sem);

    free(tp);

    return true;
  }
  return false;
}

bool thread_pool_start(thread_pool_t* const tp, void (*fun)(int, void*),
                       void* arg) {
  if(tp != NULL && fun != NULL && !tp->running) {
    tp->fun = fun;
    tp->running = true;
    tp->arg = arg;

    int val;
    sem_getvalue(&tp->sem, &val);
    while(val != 0) {
      sem_getvalue(&tp->sem, &val);
    }

    for(int i = 0; i < THREAD_POOL_SIZE; ++i) {
      thread_pool_param_t* p = malloc(sizeof(thread_pool_param_t));
      p->id = i;
      p->tp = tp;
      pthread_create(&tp->pool[i], NULL,
                     (void* (*)(void*))thread_pool_work_wrapper, p);
    }
    return true;
  }
  return false;
}

bool thread_pool_notify(thread_pool_t* const tp) {
  if(tp != NULL) {
    sem_post(&tp->sem);

    return true;
  }
  return false;
}

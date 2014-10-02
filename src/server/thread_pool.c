#include "server/thread_pool.h"

void thread_pool_work_wrapper(thread_pool_param_t* const p) {
  while(p->tp->running) {
    pthread_mutex_lock(&p->tp->mutex);
    pthread_cond_wait(&p->tp->cond, &p->tp->mutex);
    pthread_mutex_unlock(&p->tp->mutex);

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

    pthread_mutex_init(&(*tp)->mutex, NULL);
    pthread_cond_init(&(*tp)->cond, NULL);
    (*tp)->running = false;

    return true;
  }
  return false;
}

bool thread_pool_destroy(thread_pool_t* const tp) {
  if(tp != NULL) {
    thread_pool_stop(tp);

    pthread_mutex_destroy(&tp->mutex);
    pthread_cond_destroy(&tp->cond);

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
  if(tp != NULL && tp->running == true) {
    pthread_mutex_lock(&tp->mutex);
    pthread_cond_signal(&tp->cond);
    pthread_mutex_unlock(&tp->mutex);

    return true;
  }
  return false;
}
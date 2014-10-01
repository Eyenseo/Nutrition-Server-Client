#ifndef SERVER_H
#define SERVER_H

#include <semaphore.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "food_data.h"
#include "file_parser.h"
#include "int_queue.h"
#include "thread_pool.h"
#include "networking.h"

#define DEFAULT_PORT "12345"
#define BACKLOG 10

typedef struct {
  int used_fds[THREAD_POOL_SIZE];
  food_t** db;
  int db_size;
  thread_pool_t* tp;
  int_queue_t* queue;
  const char* db_filename;
  const char* port;

  pthread_mutex_t read_mutex;
  pthread_mutex_t write_mutex;
  sem_t read_sem;

  int server_fd;
  struct addrinfo hints;

  pthread_t listener;

  bool running;
} server_t;

bool server_create(server_t** const sp, const char* const db_filename,
                   const char* const port);
void server_destroy(server_t* const s);
bool server_init(server_t* const s);
bool server_start(server_t* const s);
bool server_stop(server_t* const s);
void server_listen(server_t* const s);
void server_worker(int id, server_t* s);

#endif

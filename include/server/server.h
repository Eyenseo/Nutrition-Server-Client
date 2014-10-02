#ifndef SERVER_H
#define SERVER_H

#include <semaphore.h>
#include <netdb.h>

#include "food_data.h"
#include "file_parser.h"
#include "int_queue.h"
#include "thread_pool.h"

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

  pthread_cond_t write_cond;
  pthread_mutex_t read_mutex;
  pthread_mutex_t write_mutex;
  int read_count;

  int server_fd;
  struct addrinfo hints;

  pthread_t listener;

  bool running;
} server_t;

bool server_create(server_t** const sp, const char* const db_filename,
                   const char* const port);
void server_destroy(server_t* const s);

bool server_start(server_t* const s);
bool server_start_init(server_t* const s);
void server_start_listen(server_t* const s);
bool server_stop(server_t* const s);

void server_worker(int id, server_t* s);
bool server_worker_add(server_t* const s, int client_fd);
bool server_worker_search(server_t* const s, int client_fd);

void server_read_start(server_t* const s);
void server_read_end(server_t* const s);
void server_write_start(server_t* const s);
void server_write_end(server_t* const s);

#endif

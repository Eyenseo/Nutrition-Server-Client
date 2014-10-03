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
  pthread_cond_t write_cond;
  pthread_mutex_t read_mutex;
  pthread_mutex_t write_mutex;
  pthread_t listener;
  struct addrinfo hints;
  food_t** db;
  const char* db_filename;
  const char* port;
  int_queue_t* queue;
  thread_pool_t* tp;
  int db_size;
  int read_count;
  int server_fd;
  bool running;
} server_t;

/**
 * @param  sp          serv er pointer tp store the new server to
 * @param  db_filename file name that contains the food information for the
 *                     server "database"
 *                     File Definition:
 *                       Food, Measure, Weight (g), kCal, Fat (g), Carbo(g),
 *                       Protein (g)
 *                       Note: Food name may contain comma(s)
 *                       lines starting with # are comments
 *
 *                      Example:
 *                        Lamb,Chops,Loin,Broil,Lean+Fat,2.8 oz,80,235,16,0,22
 * @param  port        Port to listen on - can be NULL
 *
 * @return             true if successful
 */
bool server_create(server_t** const sp, const char* const db_filename,
                   const char* const port);
/**
 * @param s server to destroy
 */
void server_destroy(server_t* const s);

/**
 * the functions tarts the server ak listening for new connections as answering
 * them
 *
 * @param  s server to start
 *
 * @return   true if successful
 */
bool server_start(server_t* const s);

/**
 * The function initialises the server - don't call manualy
 *
 * @param  s server to initialize
 *
 * @return   true if successful
 */
bool server_start_init(server_t* const s);

/**
 * The function listens for new connections - don't call manually
 *
 * @param s server to start listen
 */
void server_start_listen(server_t* const s);
/**
 * the function stops a server if running ak stops listening and exits every
 * connections currently held ungracefully
 *
 * @param  s server to stop
 *
 * @return   true if successful
 */
bool server_stop(server_t* const s);

/**
 * The function in the one that is passed to the thread pool to execute if work
 * is available
 *
 * @param id id of the thread
 * @param s  server that owns the thread pool
 */
void server_worker(int id, server_t* s);

/**
 * The function adds receives a food data struct from a client and adds it to
 * the "database"
 *
 * @param  s         server to to add the food data to
 * @param  client_fd client to receive the food data from
 *
 * @return           true if successful
 */
bool server_worker_add(server_t* const s, int client_fd);

/**
 * The function receives a search therm from a client and sends the result of
 * the search in the database to the client
 *
 * @param  s         server to search
 * @param  client_fd client to receive the search term from
 *
 * @return           true if successful
 */
bool server_worker_search(server_t* const s, int client_fd);

/**
 * The function is a preventing simultaneously read and writes - simultaneous
 * reads are possible. Will block if a write is requested
 *
 * @param s server to check for reading
 */
void server_read_start(server_t* const s);

/**
 * The function decreases the read count and possible unblocks a write request
 * if it was the last read
 *
 * @param s server to notify about finished read
 */
void server_read_end(server_t* const s);

/**
 * The function blocks when the read count in not zero and blocks further read
 * requests until server_write_end is called - only one read at a time
 *
 * @param s server to request write access
 */
void server_write_start(server_t* const s);

/**
 * The function unblocks all readers that where waiting for read access or write
 * access
 *
 * @param s server to notify of the finished write
 */
void server_write_end(server_t* const s);

#endif

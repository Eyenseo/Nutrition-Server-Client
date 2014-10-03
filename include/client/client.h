#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#define DEFAULT_PORT "12345"

typedef struct {
  int server_fd;
  bool running;
  const char* server_ip;
  const char* server_port;
} client_t;

/**
 * @param c    variable to save the new client to
 * @param ip   server IP address to contact (IPv4 and IPv6 possible)
 * @param port port to contact on the server
 */
void client_create(client_t** const c, const char* const ip,
                   const char* const port);

/**
 * @param c client to destroy
 */
void client_destroy(client_t* const c);

/**
 * The function tries to connect the client with a server
 *
 * @param  c client to connect
 *
 * @return   true if successful
 */
bool client_connect(client_t* const c);

/**
 * The function displays a UI for the user to interact with - blocks until
 * received start signal from server
 *
 * @param  c client to show the ui for
 *
 * @return   true if successful
 */
bool client_ui(client_t* const c);

/**
 * THe function displays an ui for the user to create new food data - blocks
 * until server response
 *
 * @param  c client to show the ui for
 *
 * @return   true if successful
 */
bool client_ui_add(client_t* const c);

/**
 * THe function shows a ui for the user to enter a search term and sends it to
 * the server - blocks until answer received
 *
 * @param  c client to show the ui for
 *
 * @return   true if successful
 */
bool client_ui_search(client_t* const c);

/**
 * The function is a wrapper to obtain a cstring from the user
 *
 * @param text text to display the user to explain what he should do
 * @param s    pointer to store the cstring to
 */
void client_ui_get_cstring(const char* const text, char** const s);

/**
 * the function is a wrapper to ask the user for a unsigned float
 *
 * @param text text to display the user to explain what he should do
 * @param n    pointer to float to store the value to
 */
void client_ui_get_ufloat(const char* const text, float* const n);

/**
 * the function is a wrapper to ask the user for a unsigned integer
 *
 * @param text text to display the user to explain what he should do
 * @param n    pointer to integer to store the value to
 */
void client_ui_get_uint(const char* const text, int* const n);

#endif

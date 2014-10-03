#ifndef NETWORKING_H
#define NETWORKING_H

#include <netdb.h>
#include <stdbool.h>

#define NETWORKING_INT_LENGTH 10

typedef enum {
  ADD,
  FAILIOR,
  QUIT,
  RESULT,
  SEARCH,
  START,
  SUCCESS
} network_intent_t;

/**
 * The function returns either the IPv6-address or IPv4-address which ever is
 * defined
 *
 * @param sa socket adress struct
 */
void* get_ip4_or_ip6(const struct sockaddr* const sa);

/**
 * The functions sends the whole char array and resumes partial sends
 *
 * @param  fd  file descriptor of the destination
 * @param  buf char array to send to the destination
 * @param  len length of the char array
 *
 * @return     true if successful
 */
bool save_send(int fd, const char* const buf, int const len);

/**
 * @param  fd file descriptor of the destination
 * @param  n  number to send
 *
 * @return    true if successful
 */
bool send_number(int const fd, int n);

/**
 * @param  fd file descriptor of the destination
 * @param  n  pointer to variable to store the received number to
 *
 * @return    true if successful
 */
bool recv_number(int const fd, int* const n);

/**
 * @param  fd file descriptor of the destination
 * @param  s  cstring to send
 *
 * @return    true if successful
 */
bool send_cstr(int const fd, const char* const s);

/**
 * @param  fd file descriptor of the destination
 * @param  s  pointer to variable to store the received cstring to
 *
 * @return    true if successful
 */
bool recv_cstr(int const fd, char** const s);

/**
 * @param  fd  file descriptor of the destination
 * @param  r   array of cstrings to send
 * @param  len array length
 *
 * @return    true if successful
 */
bool send_cstr_arr(int const fd, char** const r, int const len);

/**
 * @param  fd file descriptor of the destination
 * @param  r   pointer to the array where the cstrings are saved
 * @param  len pointer to variable to store the length of the array
 *
 * @return    true if successful
 */
bool recv_cstr_arr(int const fd, char*** const r, int* const len);

#endif

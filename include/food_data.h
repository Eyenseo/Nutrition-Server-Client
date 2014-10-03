#ifndef FOOD_DATA_H
#define FOOD_DATA_H

#include <stdlib.h>
#include <stdbool.h>

#define FOOD_DATA_TEXT_LENGTH 40
#define FOOD_DATA_INT_LENGTH 10
#define FOOD_DATA_FLOAT_LENGTH FOOD_DATA_INT_LENGTH + 4  // one dot 3 deci
#define FOOD_DATA_SERIALIZED_LENGTH \
  2 * FOOD_DATA_TEXT_LENGTH + 6 * FOOD_DATA_INT_LENGTH + FOOD_DATA_FLOAT_LENGTH

typedef struct {
  char* measure;
  char* name;
  float weight;
  int carbo;
  int fat;
  int k_cal;
  int protein;
} food_t;

/**
 * @param  foo pointer to variable to store the food struct to
 *
 * @return     true if successful
 */
bool food_create(food_t** const foo);

/**
 * @param foo food struct to destroy
 */
void food_destroy(food_t* const foo);

/**
 * The function serializes the food struct as cstring
 *
 * @param  f   food struct to be serialized
 * @param  arr pointer of initialized char array
 *
 * @return     true if successful
 */
bool food_serialize(const food_t* const f, char* const arr);

/**
 * The function deserializes a cstring to a food struct
 * @param  str cstring to be deserialized
 * @param  f   pointer to store the food struct to
 *
 * @return     true if successful
 */
bool food_deserialize(const char* const str, food_t** const f);

/**
 * THe function clears a cstring of unallowed characters eg. \n
 *
 * @param s cstring to be cleaned
 */
void food_clear_cstr(char* const s);

#endif

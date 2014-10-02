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

bool food_create(food_t** const foo);
void food_destroy(food_t* const foo);

bool food_serialize(const food_t* const f, char* const arr);
bool food_deserialize(const char* const arr, food_t** const f);

#endif

#ifndef FOOD_DATA_H
#define FOOD_DATA_H

#include <stdlib.h>

#define FOOD_DATA_TEXT_LENGTH 40
#define FOOD_DATA_INT_LENGTH 10
#define FOOD_DATA_FLOAT_LENGTH FOOD_DATA_INT_LENGTH + 4  // one dot 3 deci
#define FOOD_DATA_SERIALIZED_LENGTH \
  2 * FOOD_DATA_TEXT_LENGTH + 6 * FOOD_DATA_INT_LENGTH + FOOD_DATA_FLOAT_LENGTH;

typedef struct {
  int k_cal;
  int fat;
  int carbo;
  int protein;
  int name_length;
  int measure_length;
  float weight;
  char* name;
  char* measure;
} food_t;


food_t* food_create();
void food_destroy(food_t* const foo);

#endif

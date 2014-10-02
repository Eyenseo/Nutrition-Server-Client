#include "food_data.h"

#include <stdio.h>
#include <string.h>

food_t* food_create() {
  food_t* foo = malloc(sizeof(food_t));

  foo->k_cal = 0;
  foo->fat = 0;
  foo->carbo = 0;
  foo->protein = 0;
  foo->name_length = 0;
  foo->measure_length = 0;
  foo->weight = 0;
  foo->name = NULL;
  foo->measure = NULL;

  return foo;
}

void food_destroy(food_t* const foo) {
  if(foo->name != NULL) {
    free(foo->name);
  }
  if(foo->measure != NULL) {
    free(foo->measure);
  }
  free(foo);
}

bool food_serialize(const food_t* const f, char* const arr) {
  if(f != NULL && arr != NULL) {
    sprintf(arr, "%d,%d,%d,%d,%d,%d,%.3f,%s%s", f->carbo, f->fat, f->k_cal,
            f->measure_length, f->name_length, f->protein, f->weight,
            f->measure, f->name);
    return true;
  }
  return false;
}

bool food_deserialize(const char* const arr, food_t** const f) {
  if(f != NULL && arr != NULL) {
    *f = malloc(sizeof(food_t));
    food_t* const foo = *f;

    sscanf(arr, "%d,%d,%d,%d,%d,%d,%f,%*[^\n\0]", &foo->carbo, &foo->fat,
           &foo->k_cal, &foo->measure_length, &foo->name_length, &foo->protein,
           &foo->weight);

    int offset = 6 * FOOD_DATA_INT_LENGTH + FOOD_DATA_FLOAT_LENGTH;

    foo->measure = malloc(sizeof(char) * foo->measure_length + 1);
    strncpy(foo->measure, arr + offset, foo->measure_length);
    foo->measure[foo->measure_length] = '\0';

    offset += foo->measure_length;

    foo->name = malloc(sizeof(char) * foo->name_length + 1);
    strncpy(foo->name, arr + offset, foo->name_length);
    foo->name[foo->name_length] = '\0';

    return true;
  }
  return false;
}

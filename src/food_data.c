#include "food_data.h"

#include <stdio.h>
#include <string.h>

bool food_create(food_t** const foo) {
  *foo = malloc(sizeof(food_t));

  (*foo)->k_cal = 0;
  (*foo)->fat = 0;
  (*foo)->carbo = 0;
  (*foo)->protein = 0;
  (*foo)->weight = 0;
  (*foo)->name = NULL;
  (*foo)->measure = NULL;

  return true;
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
    sprintf(arr, "%10d%10d%10d%10ld%10ld%10d%14.3f%s%s", f->carbo, f->fat,
            f->k_cal, strlen(f->measure), strlen(f->name), f->protein,
            f->weight, f->measure, f->name);
    return true;
  }
  return false;
}

bool food_deserialize(const char* const arr, food_t** const f) {
  if(f != NULL && arr != NULL) {
    *f = malloc(sizeof(food_t));
    food_t* const foo = *f;
    int measure_length;
    int name_length;

    sscanf(arr, "%10d%10d%10d%10d%10d%10d%13f%*[^\n\0]", &foo->carbo, &foo->fat,
           &foo->k_cal, &measure_length, &name_length, &foo->protein,
           &foo->weight);

    int offset = 6 * FOOD_DATA_INT_LENGTH + FOOD_DATA_FLOAT_LENGTH;

    foo->measure = malloc(sizeof(char) * (measure_length + 1));
    strncpy(foo->measure, arr + offset, measure_length);
    foo->measure[measure_length] = '\0';

    offset += measure_length;

    foo->name = malloc(sizeof(char) * (name_length + 1));
    strncpy(foo->name, arr + offset, name_length);
    foo->name[name_length] = '\0';

    food_clear_cstr(foo->name);
    food_clear_cstr(foo->measure);

    return true;
  }
  return false;
}

void food_clear_cstr(char* const s) {
  for(unsigned long i = 0, i_end = strlen(s); i < i_end; ++i) {
    if(s[i] == '\n' || s[i] == '\t') {
      s[i] = ' ';
    }
  }
}

#ifndef FOOD_DATA_H
#define FOOD_DATA_H

#include <stdlib.h>

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

#endif

#include "server/file_parser.h"

int main(int argc, char const* argv[]) {
  food_t** arr;
  int arr_size = 0;

  file_to_food_array("../calories.csv", &arr, &arr_size);

  for(int i = 0; i < arr_size; ++i) {
    printf("%s | %s | %f | %d | %d | %d | %d\n", arr[i]->name, arr[i]->measure, arr[i]->weight,
           arr[i]->k_cal, arr[i]->fat, arr[i]->carbo, arr[i]->protein);
  }

  return 0;
}

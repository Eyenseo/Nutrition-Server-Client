#include "server/file_parser.h"

#define STRGIFY(x) STRGIFY2(x)
#define STRGIFY2(x) #x

bool count_csv_lines(FILE* f, int* lines) {
  if(f != NULL && lines != NULL) {
    long int pos = ftell(f);
    int c = fgetc(f);
    *lines = 0;

    if(pos == -1L) {
      perror("The following error occurred" STRGIFY(__FILE__) ":" STRGIFY(
          __LINE__));
      return false;
    }

    if(c != EOF && c != '#') {  // check first line
      *lines += 1;
    }

    while(EOF != (fscanf(f, "%*[^\n]"), fscanf(f, "%*c"))) {
      c = fgetc(f);

      if(c != EOF && c != '#') {  // not a comment
        *lines += 1;
      }
    }

    fseek(f, pos, SEEK_SET);

    return true;
  }
  return false;
}

void calculate_sections(const char* const line, int* const sec_arr) {
  int i = 0;

  while(line[i] != '\0') {
    if(line[i] == ',') {
      for(int j = 0; j < 6; ++j) {
        sec_arr[j] = sec_arr[j + 1];
      }
      sec_arr[6] = i;
    }
    ++i;
  }
}

bool cstring_to_food(const char* const line, food_t** const food_p) {
  if(line != NULL && food_p != NULL && line[0] != '#') {
    int sections[7];
    calculate_sections(line, sections);

    *food_p = food_create();
    food_t* foo = *food_p;

    sscanf(line + sections[2], ",%f,%d,%d,%d,%d", &foo->weight, &foo->k_cal,
           &foo->fat, &foo->carbo, &foo->protein);

    foo->name_length = sections[1];
    foo->name = malloc(sizeof(char) * 50);
    strncpy(foo->name, line, sections[1]);
    foo->name[sections[1]] = '\0';

    foo->measure_length = sections[2] - sections[1] - 1;
    foo->measure = malloc(sizeof(char) * 50);
    strncpy(foo->measure, line + sections[1] + 1, foo->measure_length);
    foo->measure[foo->measure_length] = '\0';

    return true;
  }
  return false;
}


bool file_to_food_array(const char* const filename, food_t*** const arr_p,
                        int* const length) {

  if(filename != NULL && arr_p != NULL && length != NULL) {

    FILE* f;

    f = fopen(filename, "rb");

    if(f == NULL) {
      perror("The following error occurred in line " STRGIFY(
          __FILE__) ":" STRGIFY(__LINE__));
      return false;
    }

    count_csv_lines(f, length);

    *arr_p = malloc(sizeof(food_t*) * *length);

    int c = 0;
    int i = 0;
    char* buff = malloc(sizeof(char) * FP_MAX_LINE_LENGTH + 1);

    while(i < *length) {
      if(EOF == fscanf(f, "%" STRGIFY(FP_MAX_LINE_LENGTH) "[^\n]", buff)) {
        perror("The following error occurred" STRGIFY(__FILE__) ":" STRGIFY(
            __LINE__));
        break;
      }

      if(cstring_to_food(buff, &((*arr_p)[i]))) {
        ++i;
      }

      c = fgetc(f);  // read new line or EOF
    }
    free(buff);

    fclose(f);

    return true;
  }
  return false;
}


bool food_array_to_file(const char* const filename, food_t** const arr,
                        int const length) {
  if(filename != NULL && arr != NULL) {
    FILE* f;

    f = fopen(filename, "wb");

    if(f == NULL) {
      perror("The following error occurred in line " STRGIFY(
          __FILE__) ":" STRGIFY(__LINE__));
      return false;
    }

    for(int i = 0; i < length; ++i) {
      const food_t* const foo = arr[i];

      fprintf(f, "%s,%s,%.3f,%d,%d,%d,%d", foo->name, foo->measure, foo->weight,
              foo->k_cal, foo->fat, foo->carbo, foo->protein);
    }

    fclose(f);

    return true;
  }
  return false;
}

#undef STRGIFY
#undef STRGIFY2

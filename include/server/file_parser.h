#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "food_data.h"

#define FP_MAX_LINE_LENGTH 160

bool file_parser_file_to_food_array(const char* const filename,
                                    food_t*** const arr_p, int* const length);
bool file_parser_food_array_to_file(const char* const filename,
                                    food_t** const arr_p, int const length);
bool file_parser_cstring_to_food(const char* const line, food_t** const food_p);
void file_parser_calculate_sections(const char* const line, int* const sec_arr);
bool file_parser_count_csv_lines(FILE* f, int* lines);

#endif

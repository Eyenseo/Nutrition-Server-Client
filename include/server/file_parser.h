#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "food_data.h"

#define FP_MAX_LINE_LENGTH 160

/**
 * The function will parse a csv file to food array
 *
 * File Definition:
 *   Food, Measure, Weight (g), kCal, Fat (g), Carbo(g), Protein (g)
 *   Note: Food name may contain comma(s)
 *   lines starting with # are comments
 *
 * Example:
 *   Lamb,Chops,Loin,Broil,Lean+Fat,2.8 oz,80,235,16,0,22
 *
 * @param  filename filename to read from
 * @param  arr_p    array pointer to store the array to
 * @param  length   length of the created array
 *
 * @return          true if successful
 */
bool file_parser_file_to_food_array(const char* const filename,
                                    food_t*** const arr_p, int* const length);

/**
 * The function will write a food array to a cvs file
 *
 * File Definition:
 *   Food, Measure, Weight (g), kCal, Fat (g), Carbo(g), Protein (g)
 *   Note: Food name may contain comma(s)
 *   lines starting with # are comments
 *
 * Example:
 *   Lamb,Chops,Loin,Broil,Lean+Fat,2.8 oz,80,235,16,0,22
 *
 * @param  filename filename to write to
 * @param  arr_p    array to read from
 * @param  length   length of the array
 *
 * @return          true if successful
 */
bool file_parser_food_array_to_file(const char* const filename,
                                    food_t** const arr_p, int const length);

/**
 * The function will parse a cstring (line) to a food struct
 *
 * File Definition:
 *   Food, Measure, Weight (g), kCal, Fat (g), Carbo(g), Protein (g)
 *   Note: Food name may contain comma(s)
 *   lines starting with # are comments
 *
 * Example:
 *   Lamb,Chops,Loin,Broil,Lean+Fat,2.8 oz,80,235,16,0,22
 *
 *
 * @param  line   line to parse
 * @param  food_p pointer to stroe the food struct to
 *
 * @return        true if successful
 */
bool file_parser_cstring_to_food(const char* const line, food_t** const food_p);

/**
 * The function calculates the sections of a cstring (line) from the csv file.
 *
 * @param line    line to analyse
 * @param sec_arr array of size 7 to store the sections in
 */
void file_parser_calculate_sections(const char* const line, int* const sec_arr);

/**
 * The function counts all lines that contaiin valid data in a cvs file (ak no
 * comments)
 *
 * File Definition:
 *   Food, Measure, Weight (g), kCal, Fat (g), Carbo(g), Protein (g)
 *   Note: Food name may contain comma(s)
 *   lines starting with # are comments
 *
 * Example:
 *   Lamb,Chops,Loin,Broil,Lean+Fat,2.8 oz,80,235,16,0,22
 *
 * @param  f     file to count the lines in
 * @param  lines variable to store the counted line amount in
 *
 * @return       true if successful
 */
bool file_parser_count_csv_lines(FILE* f, int* lines);

#endif

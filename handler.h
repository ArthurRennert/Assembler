#ifndef HANDLER_H
#define HANDLER_H
#define MAX_BUFFER_LENGTH 128
#define ABSOLUTE 'A'
#define RELOCATABLE 'R'
#define EXTERNAL 'E'
#define SOURCE_OP 's'
#define DESTINATION_OP 'd'
#define NO_OPERAND 'n'
#include <stdio.h>
#include <stdlib.h>	/* for malloc */
#include "line_details.h"
#include "parser.h"
#include "main_functions.h"
#include "handler_functions_for_memory.h"
#include "memory_image.h"


/**
 * file_processor is the main function of this file. It gets the file from 'main.c' and it operates all needed actions in order to process the file.
 * A pointer to the pointer of the file must be passed to the function,as well as the name of the file requested to be processed must be 
 * passed to the function.
 * 
 * @param fp - a pointer to the pointer that points to the file.
 * @param fn - the name of the file that is requested to get processed.
 * @return - 1 if file has processed error-free, otherwise 0 is returned.
 */
int file_processor(FILE** fp, const char* fn);


/**
 * find_next_line function finds where the new line starts at.
 *
 * @param fp - the pointer to the pointer which points to the file.
 */
void find_next_line(FILE** fp);


/**
 * is_long_line function checks whether the line is too long. (MAX_LINE_LENGTH defined at 'line.details.h' and is equal to 81)
 *
 * @param fp - a pointer to the pointer which points to the file.
 * @return - 1 if line is too long, otherwise 0 is returned.
 */
int is_long_line(int line_len, int white_spaces_quantt);


/**
 * is_line_ending_with_newline_char function checks whether the line is ending with a newline char.
 *
 * @param line - a pointer to the start of the line.
 * @param line_len - the length of the line.
 * @return - 1 if line is ending with newline char, otherwise 0 is returned.
 */
int is_line_ending_with_newline_char(char* line, int line_len);


/**
 * regis_to_num function returns the value of the register (in base 10, in string representation) currently proccesed in handler execution.
 *
 * Register's r0 value is 1 (in binary - 000000000001)
 * Register's r1 value is 2 (in binary - 000000000010)
 * Register's r2 value is 4 (in binary - 000000000100)
 * same rule (each register's binary representation shifts left logical once from the previous register's binary representation) applies to 
 * registers r3, r4, r5, r6, r7.

 * @param str - the string representation of the register.
 * @return - the string representation (in base 10) value of the register currently proccesed in handler execution.
 */
char* regis_to_num(char* source);


/**
 * update_DC function updates all DC values to come after all IC values in memory image.
 *
 * @param node - a pointer to the head of the list of data memory image.
 * @param IC - all DC values are getting updated according to this IC.
 */
void update_DC(mdl_ptr node, int IC);


/**
 * second_pass_label_check function checks whether all label used as operands were correctly defined.
 * The function does not return anything but if it finds any error, it triggers an error edition which is checked later in the code to determine
 * next actions.
 *
 * @param node - a pointer to the head of the list of code memory image.
 * @param fn - the name of the file currently being checked.
 */
void second_pass_label_check(mcl_ptr node, const char* fn);


#endif

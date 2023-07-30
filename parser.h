#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdarg.h>	/* use in operands_parser function */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  /* for isdigit function */
#include "line_details.h"
#define NUM_OF_OPERATIONS 16
#define NUM_OF_REGISTERS 8
#define MAX_TOKENS 50
#define MAX_DATA_TOKENS 40
#define NUM_OF_KEYWORDS 5
#define zero 48
#define nine 57
#define A 56
#define Z 90
#define a 97
#define z 122
#define ENTRY_STR ".entry"
#define EXTERN_STR ".extern"
#define STRING_STR ".string"
#define DATA_STR ".data"


/**
 * line_parser function parses the line and it returns it's type. see line-types in enum LINE_TYPE.
 * ld_ptr must be passed to the function so it stores in the struct all relevant information and together with the return value, 'handler.c' file
 * retrieves the relevant information.
 *
 * @param line - the line to parse.
 * @param ld_ptr - the pointer to line_details struct. this function stores here all relevant information for 'handler.c'.
 * @return - LINE_TYPE enum value. 
 */
LINE_TYPE line_parser(char* line, line_details* ld_ptr);


/**
 * which_addr_type function returns the addressing type enum value of the operand. See addressing_type enum definition in 'line_details.h'.
 * In case the function detected a non valid addressing type, the corresponding err enum value is set to err param.
 *
 * @param str - the string of the potential operand.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - addressing_type enum value. no_type is returned if valid addressing type hasn't detected. 
 */
addressing_type which_addr_type(char* str, ERR_VALUE* err);


/**
 * count_whitespaces function counts the whitespaces in 'str' string and returns the it's quantity.
 *
 * @param str - the string in which whitespaces will be counted.
 * @return - the quantity of whitespaces detected. 
 */
int count_whitespaces(const char* str);


/**
 * is_label function checks if 'str' is a valid label.
 * In case the function detected a non valid label, the corresponding err enum value is set to err param.
 *
 * @param str - the potential label.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if label is valid, otherwise 0 is returned. 
 */
int is_label(char* str, ERR_VALUE* err);


/**
 * is_data function checks if all string after .data is a valid.
 * In case the function detected a non valid label, the corresponding err enum value is set to err param.
 *
 * @param str - the potential array of numbers.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if array values are valid, otherwise 0 is returned. 
 */
int is_data(char* str, ERR_VALUE* err);


/**
 * is_string function checks if 'str' is a valid string. 'str' is what comes after .string.
 * In case the function detected a non valid label, the corresponding err enum value is set to err param.
 *
 * @param str - the potential string.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if string is valid, otherwise 0 is returned. 
 */
int is_string(char* str, ERR_VALUE* err);


/**
 * is_operation function checks whether 'str' is one of the operations. See ops enum definition in 'line_details.h' file.
 * All operations are: mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rts, stop.
 *
 * @param str - the string to check if it's an operation.
 * @return - ops enum value. no_op (ops enum value) is returned in case the function detected a non operation 'str'.
 */
ops is_operation(char* str);


/**
 * is_register function checks whether 'str' is a one of the registers.
 * All registers are: r0, r1, r2, r3, r4, r5, r6, r7.
 *
 * @param str - the string to check if it's a register.
 * @return - 1 if 'str' is one of the registers, otherwise 0 is returned.
 */
int is_register(char* str);


/**
 * is_keyword function checks whether 'str' is a one of the keywords.
 * All keywords are: PSW, data, string, entry, extern.
 *
 * @param str - the string to check if it's a keyword.
 * @return - 1 if 'str' is one of the registers, otherwise 0 is returned.
 */
int is_keyword(char* str);


/**
 * string_tokener function gets a line and an array of strings and it breakes the line into tokens and stores the tokens in the array.
 * The function's token delimiter is " \t".
 *
 * @param line - the line to break into tokens.
 * @param tokens - the array of strings to store the tokens.
 * @return - the total number of tokens that the function found.
 */
int string_tokener(char* line, char** tokens);


/**
 * is_potential_label function returns whether the label has ':' in the end of it. If it has ':' in the end of it, it replaces it with the string null 
 * terminator char and returns 1;
 *
 * @param lbl - the label to check.
 * @return - 1 if 'lbl' string has the char ':' in the end of it, otherwise 0 is returned.
 */
int is_potential_label(char* lbl);


/**
 * operands_parser function gets 1, 2 or 3 tokens and it checks whether the tokens are some combination of valid operands.
 * In case the function detected a non valid operan/s, the corresponding err enum value is set to err param.
 *
 * @param src - A string where to store the source operand if it's found valid.
 * @param dest - A string where to store the destination operand if it's found valid.
 * @param src_type - An enum variable where to store the source operand type if it's found valid.
 * @param dest_type - An enum variable where to store the destination operand type if it's found valid.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @param ops_num - the number of tokens being sent to the function.
 * @param the rest parameters are the tokens. 1, 2 or 3.
 * @return - the number of valid operands found. 1 or 2. 1 is returned if tokens had only 1 operand.
 */
int operands_parser(char* src, char* dest, addressing_type* src_type, addressing_type* dest_type, ERR_VALUE* err, int ops_num, ...);


/**
 * is_op_operands_match function checks whether the op and operands match.
 * In case the function detected a non valid operan/s with operation combination, the corresponding err enum value is set to err param.
 *
 * @param op - the enum value of the operand. See ops enum definition in 'line_details.h' file.
 * @param src_adr - the addressing type of the source operand. See addressing_type enum definition in 'line_details.h' file.
 * @param dest_adr - the addressing type of the destination operand. See addressing_type enum definition in 'line_details.h' file.
 * @param ops_num - the number of tokens being sent to the function.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 is returned if the op with the operand/s do match, otherwise 0 is returned.
 */
int is_op_operands_match(ops op, addressing_type* src_adr, addressing_type* dest_adr, int ops_num, ERR_VALUE* err);


/**
 * is_entry function checks whether 'str' is ".entry".
 *
 * @param str - the string to check if it's ".entry".
 * @return - 1 is returned if 'str' is ".entry", otherwise 0 is returned.
 */
int is_entry(char* str);


/**
 * is_extern function checks whether 'str' is ".extern".
 *
 * @param str - the string to check if it's ".extern".
 * @return - 1 is returned if 'str' is ".extern", otherwise 0 is returned.
 */
int is_extern(char* str);


/**
 * is_data_line function checks whether 'str' is ".data".
 *
 * @param str - the string to check if it's ".data".
 * @return - 1 is returned if 'str' is ".data", otherwise 0 is returned.
 */
int is_data_line(char* str);


/**
 * is_string_line function checks whether 'str' is ".string".
 *
 * @param str - the string to check if it's ".string".
 * @return - 1 is returned if 'str' is ".string", otherwise 0 is returned.
 */
int is_string_line(char* str);


/**
 * concat_data_tokens function concatenating data tokens into one token.
 * In case the function detected a non valid char/symbol while concatenating the tokens, the corresponding err enum value is set to err param.
 *
 * @param from - the index of the first data token in the array of tokens.
 * @param to - the index of the last data token in the array of tokens.
 * @param tkn_ptr - the array of tokens (with the data tokens in it).
 * @param str - the string where to concatenate all the data tokens to.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if all data tokens were valid, otherwise 0 is returned.
 */
int concat_data_tokens(int from, int to, char** tkn_ptr, char* concat_str, ERR_VALUE* err);


/**
 * concat_string_tokens function concatenating string tokens into one token.
 * In case the function detected a non valid char/symbol while concatenating the tokens, the corresponding err enum value is set to err param.
 * The function removes all unnecessary spaces and tabs and leaves only one space/tab between 2 words (if there were many spaces/tabs). This is done
 * for better memory management.
 *
 * @param from - the index of the first string token in the array of tokens.
 * @param num - the quantity of string tokens in they array of tokens.
 * @param tkn_ptr - the array of tokens (with the string tokens in it).
 * @param str - the string where to concatenate all the string tokens to.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if all data tokens were valid, otherwise 0 is returned.
 */
int concat_string_tokens(int from, int num, char** tkn_ptr, char* concat_str, ERR_VALUE* err);


/**
 * split_data function splits the 'data_token' into many data tokens. This function is called after the concatenating data token is valid and now
 * only inserting the data tokens to an array is needed.
 *
 * @param str - the token to break to many data tokens.
 * @param data_tokens - an array of strings where to store the data tokens. (later on atoi function will convert it to integers).
 * @param num - an address of an integer where to store the number of data tokens stored.
 */
void split_data(char* concat_str, char** data_tokens, int* num);


/**
 * copy_data function converts data tokens to integers and copies them to 'arr' array.
 *
 * @param num - the number of data tokens to be converted to integers and copied to 'arr' array.
 * @param arr - copy the data tokens to this array.
 * @param data_tokens - an array of strings where the data tokens are stored.
 */
void copy_data(int num, int* arr, char** data_tokens);


/**
 * remove_string_marks function removes the quatation marks from the begining and the end of the 'concat_str' string.
 *
 * @param len - the length of 'concat_str' string.
 * @param concat_str - quatation marks will be removed from this string.
 */
void remove_string_marks(int len, char* concat_str);


/**
 * remove_first_char function removes the removes the first char in 'str' string.
 *
 * @param len - the length of 'str' string.
 * @param str - the first char of this string will be removed.
 */
void remove_first_char(int len, char* str);


#endif

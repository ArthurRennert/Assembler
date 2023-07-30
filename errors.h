#ifndef ERRORS_H
#define ERRORS_H
#include <stdlib.h> /* for malloc */
#include <stdio.h>  /* for printf */
#include <string.h> /* for strlen */
#include "line_details.h"   /* for ERR_VALUE enum */
#define NUM_OF_ERRORS 48    /* Number of all possible errors of the 'assembler' */


/* error_node struct pointer definition */
typedef struct error_node * err_ptr;

/* error_node struct definition */
typedef struct error_node
{
	int line;   /* holds the line number in which the error has occurred */
	char* str_error;    /* holds the error description */
	char* file_name;    /* holds the name of the file in which the error has occurred */
	err_ptr next;   /* a pointer to next error node */
}error_node;


/**
 * add_error function adds an error node to the errors-linked-list.
 * ERR_VALUE must be passed to the function for the node to hold the exact description of the error. See ERR_VALUE enum definition in 'line_details.h'.
 * Both 'head' and 'curr' pointers (each is a pointer to pointer. for example. 'head' points to the pointer of the head of the errors-linked-list.) must be passed to the
 * function so it (the function) adds the requested node correctly. In addition, for the node to hold as more information as possible, the file name must be passed to the function.
 *
 * @param ln - the line number in which the error has occurred.
 * @param err - the enum value of the error.
 * @param curr - the pointer to the pointer that points to the current error node.
 * @param head - the pointer to the pointer that points to the head error node.
 * @param fn - the name of the file in which the error has occurred.
 */
void add_error(int ln, ERR_VALUE err, err_ptr * curr, err_ptr * head, const char* fn);


/**
 * print_errors function prints all errors that were found in the execution.
 * The head node must be passed to the function so that it (the function) starts to print from the start of the errors-linked-list to the end of it.
 *
 * @param node - the (head) node of the errors-linked-list.
 */
void print_errors(err_ptr node);


/**
 * free_error_nodes function frees all error nodes that were created during the execution.
 * The head (pointer to pointer) of the errors-linked-list must be passed to the function so it (the function) frees all nodes from the start of the errors-linked-list to the end of it.
 *
 * @param head_node - the pointer to the pointer that points to the errors-linked-list.
 */
void free_error_nodes(err_ptr * head_node);


#endif

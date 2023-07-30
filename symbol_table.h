#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <stdio.h>
#include <stdlib.h>	/* for malloc */
#include <string.h>
#include "line_details.h"
#include "main_functions.h"	/* for get_file_name function */
#include "handler_functions_for_memory.h"	/* for get_curr_line function */
#define MAX_LABEL_LENGTH 31
#define ENT_EXTENSION ".ent"


/* a pointer to struct symb */
typedef struct symb* symb_ptr;

/* a struct that represents a node in the symbol table (list) */
typedef struct symb
{
	char label[MAX_LABEL_LENGTH + 1];	/* +1 for string terminator char */
	int addr;
	attribute attr;
	symb_ptr next;
}symb;


/**
 * insert function creates a node and adds it to the symbol table (list), if list is still empty, it's being created upon first node creation.
 * The pointers to 'head' and 'curr' must be passed to the function for the function to create the requested node correctly.
 * In case the function detected a non valid addressing type, the corresponding err enum value is set to err param.
 *
 * @param lbl - the label to add to the symbol table.
 * @param head - the pointer to the pointer that points to the head-symbol-table-linked-list.
 * @param curr - the pointer to the pointer that points to the current-symbol-table-linked-list.
 * @param IC - the number line in the file where the label is defined.
 * @param att - the attribute of the label. (attribute enum is defined in 'line_details.h').
 * @param err - a pointer to err. (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 */
void insert(char* lbl, symb_ptr * head, symb_ptr * curr, int IC, attribute att, ERR_VALUE* err);


/**
 * search_lbl function searches if a label already exists in the symbol table (list).
 *
 * @param label - the label to search in the symbol table.
 * @param head - the pointer to the head-symbol-table-linked-list.
 * @param temp - the pointer to the pointer where to store the node address where the label is found (if found).
 * @return - 1 if label found in the symbol table, otherwise 0 is returned.
 */
int search_lbl(char* label, symb_ptr head, symb_ptr * temp);


/**
 * second_pass_lbl function searches if a label exists in the symbol table (list) and if the label exists, the function returns the address of the label.
 * This function must be used ONLY if no errors were found during the execution of the 'assembler'.
 *
 * @param label - the label to search in the symbol table.
 * @param head - the pointer to the head-symbol-table-linked-list.
 * @return - the address of the label. If label was not found, -1 is returned.
 */
int second_pass_lbl(char* label, symb_ptr head);


/**
 * is_extern_symb function checks whether the label is external. If the label is external, 1 is returned, otherwise 0 is returned.
 *
 * @param label - the label to check if it's an external label.
 * @param head - the pointer to the head-symbol-table-linked-list.
 * @return - 1 is returned if the label is external, otherwise 0 is returned.
 */
int is_extern_symb(char* label, symb_ptr head);


/**
 * free_symb_list function frees all symbol table (list) nodes that were created during the execution.
 * The head (pointer to pointer) of the symbol-table-list must be passed to the function so it (the function) frees all nodes from the start of the 
 * symbol-table-list to the end of it.
 *
 * @param head_node - the pointer to the pointer that points to the symbol-table-list.
 */
void free_symb_list(symb_ptr* head_node);


/**
 * create_entry_file function creates an output entry file.
 * This function must be used ONLY if no errors were found during the execution of the 'assembler'.
 *
 * @param head_symb - the pointer to the head-symbol-table-linked-list.
 * @param fn - the name of the file of which the output file is being created.
 */
void create_entry_file(symb_ptr node, const char* fn);


/**
 * was_entry_label_as_operand function checks whether any entry label was used as an operand in an instruction.
 *
 * @param head_symb - the pointer to the head-symbol-table-linked-list.
 * @return - 1 If any entry label was used as an operand in an instruction, otherwise 0 is returned.
 */
int was_entry_label_as_operand(symb_ptr head_symb);


/**
 * were_all_entry_labels_defined function checks whether all entry labels defined correctly.
 * If not, corresponding errors are added by the function.
 *
 * @param head_symb - the pointer to the head-symbol-table-linked-list.
 */
void were_all_entry_labels_defined(symb_ptr head_symb);


#endif

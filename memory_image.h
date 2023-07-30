#ifndef MEMORY_IMAGE_H
#define MEMORY_IMAGE_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "line_details.h"
#include "handler_functions_for_memory.h"
#define OP_TABLE_LINES 16
#define OPCODE_SIZE 4
#define FUNCT_SIZE 4
#define SRC_OPERAND_SIZE 2
#define DEST_OPERAND_SIZE 2
#define OB_EXTENSION ".ob"
#define EXT_EXTENSION ".ext"


/* union_type enum is used to determine which code image memory line needs to be created */
enum memory_line_type {line_type_number, line_type_bits, direct_operand, relative_operand, no_ops};


/* struct label is needed when an operand is a label (direct or relative) */
typedef struct label {
	char* lbl;
	int lbl_line;
	char op_type;	/* s - label as source operand, d - label as destination operand */
} label;


/* a struct of a typical instruction line */
typedef struct instruction {
	char dest_addr[DEST_OPERAND_SIZE + 1]; /* +1 for string terminator char */
	char src_addr[SRC_OPERAND_SIZE + 1];    /* +1 for string terminator char */
	char funct[FUNCT_SIZE + 1];     /* +1 for string terminator char */
	char opcode[OPCODE_SIZE + 1];   /* +1 for string terminator char */
} instruction;



/* a pointer struct to a memory_code_line struct */
typedef struct memory_code_line* mcl_ptr;


/* a struct of a code memory image line, the union line_type in it "plays a role" of struct 'line_bits' or of struct 'number_ascii_register' */
typedef struct memory_code_line {
	int IC;
	enum memory_line_type mlt;
	instruction* ins;
	char* mem_val;
	char memory_type;	/* Absolute, Relocatable, External */
	label* l;
	mcl_ptr next;
} memory_code_line;


/* a pointer struct to a memory_data_line struct */
typedef struct memory_data_line* mdl_ptr;


/* a struct of a typical data line, it holds a decimal number, ascii value or a register value (the value of the register and not whats in it)  */
typedef struct memory_data_line {
	int DC;
	char* mem_val;
	char memory_type;	/* Absolute, Relocatable, External */
	mdl_ptr next;
} memory_data_line;


/**
 * insert_memory_code_line function creates a memory-code-node and adds it to the list, if list is still empty, it's being created upon first node creation.
 * The node after creation has all needed information in it about how it should appear in memory code image.
 *
 * @param head - the pointer to the pointer that points to the code-image-head-linked-list.
 * @param curr - the pointer to the pointer that points to the code-image-current-linked-list.
 * @param line_num - the line number as it should appear in code memory image.
 * @param t - the enum value of the line. see enum union_type in 'memory_image.h' file.
 * @param mt - the memory type of the line. possible inputs: ABSOLUTE, ABSOLUTE, EXTERNAL.
 * @param op_t - the operand type of the line, source or destination. possible inputs: SOURCE_OP for source operand, DESTINATION_OP for destination operand.
 */
void insert_memory_code_line(mcl_ptr * head, mcl_ptr * curr, int line_num, enum memory_line_type t, char mt, char op_t);


/**
 * insert_memory_data_line function creates a memory-data-node and adds it to the list, if list is still empty, it's being created upon first node creation.
 *
 * @param head - the pointer to the pointer that points to the data-image-head-linked-list.
 * @param curr - the pointer to the pointer that points to the data-image-current-linked-list.
 * @param line_num - the line number as it should appear in data (with code lines offset) memory image.
 * @param val - the value of the line. possible inputs: A decimal number or an ASCII of a char.
 * @param mt - the memory type of the line. possible inputs: ABSOLUTE, ABSOLUTE, EXTERNAL.
 */
void insert_memory_data_line(mdl_ptr * head, mdl_ptr * curr, int line_num, int val, char mt);


/**
 * create_image_hexa_file function creates an output hexe file of both code and data image.
 * This function must be called ONLY if no errors were found during the execution of the 'assembler'.
 *
 * @param head_mcl_node - the pointer to the code-image-head-linked-list.
 * @param head_mdl_node - the pointer to the data-image-head-linked-list.
 * @param fn - the name of the file of which the output file is being created.
 * @param IC - the total lines of code were processed. 
 * @param DC - the total lines of data were processed. 
 */
void create_image_hexa_file(mcl_ptr head_mcl_node, mdl_ptr head_mdl_ptr, const char* fn, int IC, int DC);


/**
 * create_extern_file function creates an output extern file.
 * This function must be called ONLY if no errors were found during the execution of the 'assembler'.
 *
 * @param node_mcl_ptr - the pointer to the code-image-head-linked-list.
 * @param fn - the name of the file of which the output file is being created.
 */
void create_extern_file(mcl_ptr node_mcl_ptr, const char* fn);


/**
 * was_extern_label_as_operand function checks whether any extern label was used as an operand in an instruction.
 *
 * @param node_mcl_ptr - the pointer to the code-image-head-linked-list.
 * @return - 1 If any extern label was used as an operand in an instruction, otherwise 0 is returned.
 */
int was_extern_label_as_operand(mcl_ptr node_mcl_ptr);


/**
 * free_code_image_nodes function frees all code image nodes that were created during the execution.
 * The pointer to the pointer to the head-code-image-linked-list must be passed to the function in order to free the list correctly.
 *
 * @param head_node - the pointer to the pointer that points to the head-code-image-linked-list.
 */
void free_code_image_nodes(mcl_ptr * head_node);


/**
 * free_data_image_nodes function frees all data image nodes that were created during the execution.
 * The pointer to the pointer to the head-data-image-linked-list must be passed to the function in order to free the list correctly.
 *
 * @param head_node - the pointer to the pointer that points to the head-data-image-linked-list.
 */
void free_data_image_nodes(mdl_ptr * head_node);


#endif


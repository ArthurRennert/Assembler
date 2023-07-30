/**
 * This file contains all functions and information related to memory image handling in the 'assembler'.
 *
 * @author - Arthur Losnikov
 * Assembler Project
 */

#include "memory_image.h"
#include "emergency_free_mem.h"

/**
 * insert_memory_code_line function creates a memory-code-node and adds it to the list, if list is still empty, it's being created upon first node creation.
 *
 * @param head - the pointer to the pointer that points to the code-image-head-linked-list.
 * @param curr - the pointer to the pointer that points to the code-image-current-linked-list.
 * @param line_num - the line number as it should appear in code memory image.
 * @param t - the enum value of the line.
 * @param mt - the memory type of the line. 
 * @param op_t - the operand type of the line, source or destination. 
 */
void insert_memory_code_line(mcl_ptr * head, mcl_ptr * curr, int line_num, enum memory_line_type t, char mt, char op_t)
{
	mcl_ptr p = NULL;

	/* Array of all operations values and their funct values */
	static struct opcodes {
		ops op;
		char* funct;
		char* opcode;
	} opcode_table[OP_TABLE_LINES] = {

	{mov, "0000", "0000"},
	{cmp, "0000", "0001"},
	{add, "1010", "0010"},
	{sub, "1011", "0010"},
	{lea, "0000", "0100"},
	{clr, "1010", "0101"},
	{not, "1011", "0101"},
	{inc, "1100", "0101"},
	{dec, "1101", "0101"},
	{jmp, "1010", "1001"},
	{bne, "1011", "1001"},
	{jsr, "1100", "1001"},
	{red, "0000", "1100"},
	{prn, "0000", "1101"},
	{rts, "0000", "1110"},
	{stop, "0000", "1111"}

	};

	static struct addressing_types_binary {
		addressing_type at;
		char* binary_representation;
	} addressing_types_binary_table[NUMBER_OF_ADDRESSING_TYPES] = {
		{immediate, "00"},
		{direct, "01"},
		{relative, "10"},
		{regis_direct, "11"}
	};

	p = malloc(sizeof(memory_code_line));

	/* verify if malloc worked as expected */
	if(p == NULL)
	{
		fprintf(stderr, "Fatal error: failed to allocate memory.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	p -> l = NULL;
	p -> mem_val = NULL;
	p -> ins = NULL;

	if(t == line_type_bits || t == no_ops) {
		p -> ins = malloc(sizeof(instruction));

		/* verify if malloc worked as expected */
		if(p -> ins == NULL)
		{
			free(p);
			fprintf(stderr, "Fatal error: failed to allocate memory.\n");
			emergency_free_mem();
			exit(EXIT_FAILURE);
		}
	}

	if(t == direct_operand || t == relative_operand || t == line_type_number) {
		p -> mem_val = malloc(sizeof(char) * (WORD_IN_MACHINE + 1));	/* +1 for string terminating char */

		/* verify if malloc worked as expected */
		if(p -> mem_val == NULL)
		{
			free(p -> ins);
			free(p);
			fprintf(stderr, "Fatal error: failed to allocate memory.\n");
			emergency_free_mem();
			exit(EXIT_FAILURE);
		}
	}
	
	p -> l = NULL;	

	/* if line is an instruction line */
	if(t == line_type_bits) {
		strcpy(p -> ins -> opcode, opcode_table[get_op()].opcode);
		strcpy(p -> ins -> funct, opcode_table[get_op()].funct);
		strcpy(p -> ins -> src_addr, addressing_types_binary_table[get_src_type()].binary_representation);
		strcpy(p -> ins -> dest_addr, addressing_types_binary_table[get_dest_type()].binary_representation);

	}

	/* if line is a number (immediate value or register direct */
	else if(t == line_type_number) {
		strcpy(p -> mem_val, get_number());
	}

	/* if line is label as direct or as relative */
	else if(t == direct_operand || t == relative_operand) {
		p -> l = malloc(sizeof(label));
		if(p -> l == NULL)
		{
			free(p);
			fprintf(stderr, "Fatal error: failed to allocate memory.\n");
			emergency_free_mem();
			exit(EXIT_FAILURE);
		}
		p -> l -> lbl = NULL;

		p -> l -> lbl = malloc(strlen(get_label()) + 1);	/* +1 for the null terminator char */

		/* verify if malloc worked as expected */
		if(p -> l -> lbl == NULL)
		{
			free(p -> l);
			free(p);
			fprintf(stderr, "Fatal error: failed to allocate memory.\n");
			emergency_free_mem();
			exit(EXIT_FAILURE);
		}

		strcpy(p -> l -> lbl, get_label());
		p -> l -> lbl_line = get_curr_line();
		p -> l -> op_type = op_t;
	}

	/* if line is instruction line without operands */
	else if(t == no_ops) {
		strcpy(p -> ins -> opcode, opcode_table[get_op()].opcode);
		strcpy(p -> ins -> funct, opcode_table[get_op()].funct);
		strcpy(p -> ins -> src_addr, addressing_types_binary_table[immediate].binary_representation);
		strcpy(p -> ins -> dest_addr, addressing_types_binary_table[immediate].binary_representation);
	}


	p -> memory_type = mt;
	p -> IC = line_num;
	p -> mlt = t;
	p -> next = NULL;

	/* if linked list is still empty */
	if(*head == NULL)
	{
		(*head) = p;
		(*curr) = p;
	}

	/* if linked list is not empty */
	else
	{
		(*curr) -> next = p;
		(*curr) = p;
	}
}


/**
 * insert_memory_data_line function creates a memory-data-node and adds it to the list, if list is still empty, it's being created upon first node creation.
 *
 * @param head - the pointer to the pointer that points to the data-image-head-linked-list.
 * @param curr - the pointer to the pointer that points to the data-image-current-linked-list.
 * @param line_num - the line number as it should appear in data (with code lines offset) memory image.
 * @param val - the value of the line.
 * @param mt - the memory type of the line.
 */
void insert_memory_data_line(mdl_ptr * head, mdl_ptr * curr, int line_num, int val, char mt)
{
	mdl_ptr p = NULL;

	p = malloc(sizeof(memory_data_line));

	/* verify if malloc worked as expected */
	if(p == NULL)
	{
		fprintf(stderr, "Fatal error: failed to allocate memory.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	p -> mem_val = malloc(sizeof(char) * (WORD_IN_MACHINE + 1));	/* +1 for string terminating char */

	/* verify if malloc worked as expected */
	if(p -> mem_val == NULL)
	{
		free(p);
		fprintf(stderr, "Fatal error: failed to allocate memory.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	sprintf(p -> mem_val, "%d", val);
	p -> memory_type = mt;
	p -> DC = line_num;
	p -> next = NULL;

	/* if linked list is still empty */
	if(*head == NULL)
	{
		(*head) = p;
		(*curr) = p;
	}
	/* if linked list is not empty */
	else
	{
		(*curr) -> next = p;
		(*curr) = p;
	}
}


/**
 * create_image_hexa_file function creates an output hexe file of both code and data image.
 *
 * @param head_mcl_node - the pointer to the code-image-head-linked-list.
 * @param head_mdl_node - the pointer to the data-image-head-linked-list.
 * @param fn - the name of the file of which the output file is being created.
 * @param IC - the total lines of code were processed. 
 * @param DC - the total lines of data were processed. 
 */
void create_image_hexa_file(mcl_ptr head_mcl_node, mdl_ptr head_mdl_node, const char* fn, int IC, int DC)
{
	char file_name[MAX_FILE_NAME_LENGTH + 4];	/* +1 for string terminator char, +3 for .ob extension */
	char temp[WORD_IN_MACHINE + 1];	/* +1 for string terminator char */
	FILE* fp = NULL;
	int res;

    if(head_mcl_node == NULL && head_mdl_node == NULL) {    /* an empty file */
        return;
    }
    
	strcpy(file_name, fn);	
	file_name[strlen(file_name) - 3] = '\0'; /* remove .as extension */
	strcat(file_name, OB_EXTENSION);	/* add .ob extension */

	fp = fopen(file_name, "w+");

	if(fp == NULL) {
		printf("Could not create object file.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%d %d\n", IC, DC);	/* print IC and DC in top of the file */

	while(head_mcl_node != NULL)
	{
		if(head_mcl_node -> mlt == line_type_bits || head_mcl_node -> mlt == no_ops) {
		strcpy(temp, head_mcl_node -> ins -> opcode);
		strcat(temp, head_mcl_node -> ins -> funct);
		strcat(temp, head_mcl_node -> ins -> src_addr);
		strcat(temp, head_mcl_node -> ins -> dest_addr);
		res = (int) strtol(temp, NULL, 2);

		fprintf(fp, "%04d %03X %c\n",head_mcl_node -> IC, res, head_mcl_node -> memory_type);
		}
		else if(head_mcl_node -> mlt == line_type_number || head_mcl_node -> mlt == direct_operand ||
					head_mcl_node -> mlt == relative_operand) {
			res = (int) strtol(head_mcl_node -> mem_val, NULL, 10);
			fprintf(fp, "%04d %03X %c\n",head_mcl_node -> IC, (res & 0xFFF), head_mcl_node -> memory_type);
		}
		head_mcl_node = head_mcl_node -> next;
	}

	while(head_mdl_node != NULL)
	{
		res = (int) strtol(head_mdl_node -> mem_val, NULL, 10);
		fprintf(fp, "%04d %03X %c\n",head_mdl_node -> DC, (res & 0xFFF), head_mdl_node -> memory_type);
		head_mdl_node = head_mdl_node -> next;
	}

	printf("\nFile '%s' has been created successfully!", file_name);

	fclose(fp);	
}


/**
 * create_extern_file function creates an output extern file.
 *
 * @param node_mcl_ptr - the pointer to the code-image-head-linked-list.
 * @param fn - the name of the file of which the output file is being created.
 */
void create_extern_file(mcl_ptr node_mcl_ptr, const char* fn)
{
	FILE* fp = NULL;
	char file_name[MAX_FILE_NAME_LENGTH + 5];	/* +1 for string terminator char, +4 for .ext extension */

	strcpy(file_name, fn);	
	file_name[strlen(file_name) - 3] = '\0'; /* remove .as extension */
	strcat(file_name, EXT_EXTENSION);	/* add .ext extension */

	fp = fopen(file_name, "w+");

	if(fp == NULL) {
		printf("Could not create externals file.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	while(node_mcl_ptr != NULL)
    {
		if(node_mcl_ptr -> memory_type == 'E') {
			fprintf(fp, "%s %04d\n", node_mcl_ptr -> l -> lbl, node_mcl_ptr -> IC);
		}
		node_mcl_ptr = node_mcl_ptr -> next;
	}

	printf("\nFile '%s' has been created successfully!", file_name);

	fclose(fp);
}


/**
 * was_extern_label_as_operand function checks whether any extern label was used as an operand in an instruction.
 *
 * @param node_mcl_ptr - the pointer to the code-image-head-linked-list.
 * @return - 1 If any extern label was used as an operand in an instruction, otherwise 0 is returned.
 */
int was_extern_label_as_operand(mcl_ptr node_mcl_ptr)
{
    while(node_mcl_ptr != NULL)
    {
        if(node_mcl_ptr -> memory_type == 'E') {
            return 1;
        }
        node_mcl_ptr = node_mcl_ptr -> next;
    }
    return 0;
}


/**
 * free_code_image_nodes function frees all code image nodes that were created during the execution.
 *
 * @param head_node - the pointer to the pointer that points to the code-image-linked-list.
 */
void free_code_image_nodes(mcl_ptr * head_node)
{
	mcl_ptr p;

	while(*head_node != NULL)
	{
		p = *head_node;
		*head_node = p -> next;


		if((p -> l) != NULL) {/* free label string that was allocated */
			free(p -> l -> lbl);
			p -> l -> lbl = NULL;
			free(p -> l);
			p -> l = NULL;
		}

		if(p -> ins != NULL) {
			free(p -> ins);
			p -> ins = NULL;
		}

		if(p -> mem_val != NULL) {
			free(p -> mem_val);
			p -> mem_val = NULL;
		}
		free(p);
		p = NULL;
	}
}


/**
 * free_data_image_nodes function frees all data image nodes that were created during the execution.
 *
 * @param head_node - the pointer to the pointer that points to the data-image-linked-list.
 */
void free_data_image_nodes(mdl_ptr * head_node)
{
	mdl_ptr p;

	while(*head_node != NULL)
	{
		p = *head_node;
		*head_node = p -> next;

		if(p -> mem_val != NULL) {
			free(p -> mem_val);
			p -> mem_val = NULL;
		}
		free(p);
		p = NULL;
	}
}

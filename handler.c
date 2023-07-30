/**
 * This file contains all functions related to all-operations handling in the 'assembler'. It (the file) does so by "interacting" with 'main.c'
 * 'memory_image.c', 'parser.c', 'errors.c' and 'symbol_table.c' (through 'main.c').
 * This file triggers all error, symbol, memory_image nodes' creation during the execution of the 'assembler'.
 * This file sends every line of the input file (that comes from 'main.c') to function line_parser (located in 'parser.h') and after line_parser function
 * parses the line, 'handler.c' file processes the output of line_parser.
 *
 * @author - Arthur Losnikov
 * Assembler Project
 */

#include "handler.h"
#include "emergency_free_mem.h"

static int curr_line;  /* current line in currently processed file */
line_details* ptr = NULL;   /* line details struct will be initialized later in the code for saving the processed line relevant information */
mcl_ptr head_mcl_ptr;  /* the head pointer to the memory_code_line list's node (the list is defined in 'memory_image.h') */
mdl_ptr head_mdl_ptr;  /* the head pointer to the memory_data_line list's node (the list is defined in 'memory_image.h') */

/**
 * file_processor is the main function of this file. It gets the file from 'main.c' and it operates all needed actions in order to process the file.
 *
 * @param fp - a pointer to the pointer that points to the file.
 * @param fn - the name of the file that is requested to get processed.
 * @return - 1 if file has processed error-free, otherwise 0 is returned.
 */
int file_processor(FILE** fp, const char* fn)
{
   /* variables definitions */
	FILE* file; /* the pointer to the currently processed file */
	const char* file_name;  /* the name of the currently processed file */
	int IC;  /* the instruction counter */
	int DC;  /* the data counter */
	int line_len;  /* the length of the currently processed line */
	char buffer[MAX_BUFFER_LENGTH];  /* currently processed line is stored in this buffer */
	LINE_TYPE type;  /* used in 'switch case' in this funtion (file_processor function) as a result value from line_parser (in 'parser.c') function */
	int i;  /* reusable index variable for loops in this file */

	mcl_ptr curr_mcl_ptr;  /* the current pointer to the memory_line list's node (the list is defined in 'memory_image.h') */
	mdl_ptr curr_mdl_ptr;  /* the current pointer to the memory_line list's node */
   
   /* variables initializations */
	file = *fp;
	file_name = fn;
	IC = 100;
	DC = 0;
	curr_line = 1;
	head_mcl_ptr = NULL;
	curr_mcl_ptr = NULL;
	head_mdl_ptr = NULL;
	curr_mdl_ptr = NULL;
	type = UNDEFINED_LINE;

	ptr = malloc(sizeof(line_details));

   /* verify if malloc worked as expected */
	if(ptr == NULL)
    {
		fprintf(stderr, "Fatal error: failed to allocate memory.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

   /* make sure source and destination operands are initialized as no_type */
	ptr -> src_adr = no_type;
	ptr -> dest_adr = no_type;

	while(fgets(buffer, MAX_BUFFER_LENGTH, file) != NULL)
	{

		/* START - line length handling */
		line_len = strlen(buffer);
		ptr -> err = NO_ERRORS;  /* make sure its initialized as NO_ERRORS */
		if(!feof(*fp)) {
			if(is_long_line(line_len, count_whitespaces(buffer))) {
				error_controller(curr_line, LONG_LINE, file_name);  /* add error */
				if(!is_line_ending_with_newline_char(buffer, line_len)) {
					find_next_line(fp);  /* line is too long, find where next line starts */
				}
				curr_line++;
				continue;
			}
		}
		/* END - line length handling */


		type = line_parser(buffer, ptr);

       /* this 'switch case' gets a result from line_parser funtion (in 'parser.c') and handles it according to the result */
		switch(type) {
			case EMPTY:  /* Result from line_parser: an empty line */
			{
				curr_line++;
				continue;
			}

			case COMMENT:  /* Result from line_parser: a comment line */
			{
				curr_line++;
				continue;
			}

			case ENTRY:  /* Result from line_parser: an entry line */
			{
				symb_controller(ptr -> label, IC, entry, &(ptr -> err));
				break;
			}

			case EXTERN:  /* Result from line_parser: an extern line */
			{
				symb_controller(ptr -> label, IC, external, &(ptr -> err));
				break;
			}

			case GUIDANCE_DATA_WITH_LABEL: /* Result from line_parser: a line that contains a label with data */
			{
				symb_controller(ptr -> label, DC, data, &(ptr -> err));
				if(!were_errors())
				{
					for(i = 0; i < ptr -> num_in_data_array; i++) {
						insert_memory_data_line(&head_mdl_ptr, &curr_mdl_ptr, DC, ptr -> data[i], ABSOLUTE);
						DC++;
					}
				}

				break;
			}

			case GUIDANCE_DATA_WITHOUT_LABEL:  /* Result from line_parser: a line that contains only data */
			{
				for(i = 0; i < ptr -> num_in_data_array; i++) {
					insert_memory_data_line(&head_mdl_ptr, &curr_mdl_ptr, DC, ptr -> data[i], ABSOLUTE);
					DC++;
				}

				break;
			}		

			case GUIDANCE_STRING_WITH_LABEL:  /* Result from line_parser: a line that contains a label with string */
			{
				symb_controller(ptr -> label, DC, data, &(ptr -> err));
                
				for(i = 0; i < strlen(ptr -> string); i++) {
					insert_memory_data_line(&head_mdl_ptr, &curr_mdl_ptr, DC, ptr -> string[i], ABSOLUTE);
					DC++;
				}
				insert_memory_data_line(&head_mdl_ptr, &curr_mdl_ptr, DC, 0, ABSOLUTE);	/* string null terminator */
				DC++;

				break;
			}

			case GUIDANCE_STRING_WITHOUT_LABEL:  /* Result from line_parser: a line that contains only string */
			{
				for(i = 0; i < strlen(ptr -> string); i++) {
					insert_memory_data_line(&head_mdl_ptr, &curr_mdl_ptr, DC, ptr -> string[i], ABSOLUTE);
					DC++;
				}

				break;
			}

			case INSTRUCTION_WITH_LABEL_2_OPERANDS:  /* Result from line_parser: a line that contains an operation, a label and 2 operands */
			{
				symb_controller(ptr -> label, IC, code, &(ptr -> err));

				/* START - OPERATION TO MEMORY */
				insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_bits, ABSOLUTE, NO_OPERAND);
				/* END - OPERATION TO MEMORY */

				IC++;


				/* START - SOURCE TO MEMORY */
				if(ptr -> src_adr == immediate) {
					strcpy(ptr -> num_to_mem, ptr -> source);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, SOURCE_OP);
				}
				else if(ptr -> src_adr == direct) {
					strcpy(ptr -> lbl_to_mem, ptr -> source);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, direct_operand, RELOCATABLE, SOURCE_OP);
				}
				else if(ptr -> src_adr == regis_direct) {
					strcpy(ptr -> num_to_mem, regis_to_num(ptr -> source));
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, SOURCE_OP);
				}
				/* END - SOURCE TO MEMORY */

				IC++;

				/* START - DESTINATION TO MEMORY */
				if(ptr -> dest_adr == immediate) {
					strcpy(ptr -> num_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == direct) {
					strcpy(ptr -> lbl_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, direct_operand, RELOCATABLE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == regis_direct) {
					strcpy(ptr -> num_to_mem, regis_to_num(ptr -> destination));
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, DESTINATION_OP);
				}
				/* END - DESTINATION TO MEMORY */

				IC++;

				break;
			}

			case INSTRUCTION_WITH_LABEL_1_OPERAND:  /* Result from line_parser: a line that contains an operation, a label and 1 operand */
			{
				symb_controller(ptr -> label, IC, code, &(ptr -> err));
                
				ptr -> src_adr = 0;

				/* START - OPERATION TO MEMORY */
				insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_bits, ABSOLUTE, NO_OPERAND);
				/* END - OPERATION TO MEMORY */

				IC++;

				/* START - DESTINATION TO MEMORY */
				if(ptr -> dest_adr == immediate) {
					strcpy(ptr -> num_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == direct) {
					strcpy(ptr -> lbl_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, direct_operand, RELOCATABLE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == regis_direct) {
					strcpy(ptr -> num_to_mem, regis_to_num(ptr -> destination));
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == relative) {
					strcpy(ptr -> lbl_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, relative_operand, ABSOLUTE, DESTINATION_OP);
				}
				/* END - DESTINATION TO MEMORY */

				IC++;

				break;
			}

			case INSTRUCTION_WITH_LABEL_NO_OPERANDS:  /* Result from line_parser: a line that contains an operation and a label */
			{
				symb_controller(ptr -> label, IC, code, &(ptr -> err));
             
				/* START - OPERATION TO MEMORY */
				insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, no_ops, ABSOLUTE, NO_OPERAND);
				/* END - OPERATION TO MEMORY */

				IC++;

				break;
			}

			case INSTRUCTION_WITHOUT_LABEL_2_OPERANDS:  /* Result from line_parser: a line that contains an operation and 2 operands */
			{
				/* START - OPERATION TO MEMORY */
				insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_bits, ABSOLUTE, NO_OPERAND);
				/* END - OPERATION TO MEMORY */

				IC++;

				/* START - SOURCE TO MEMORY */
				if(ptr -> src_adr == immediate) {
					strcpy(ptr -> num_to_mem, ptr -> source);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, SOURCE_OP);
				}
				else if(ptr -> src_adr == direct) {
					strcpy(ptr -> lbl_to_mem, ptr -> source);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, direct_operand, RELOCATABLE, SOURCE_OP);
				}
				else if(ptr -> src_adr == regis_direct) {
					strcpy(ptr -> num_to_mem, regis_to_num(ptr -> source));
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, SOURCE_OP);
				}
				/* END - SOURCE TO MEMORY */

				IC++;

				/* START - DESTINATION TO MEMORY */
				if(ptr -> dest_adr == immediate) {
					strcpy(ptr -> num_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == direct) {
					strcpy(ptr -> lbl_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, direct_operand, RELOCATABLE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == regis_direct) {
					strcpy(ptr -> num_to_mem, regis_to_num(ptr -> destination));
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, DESTINATION_OP);
				}
				/* END - DESTINATION TO MEMORY */

				IC++;

				break;
			}

			case INSTRUCTION_WITHOUT_LABEL_1_OPERAND:  /* Result from line_parser: a line that contains an operation and 1 operand */
			{
				ptr -> src_adr = 0;

				/* START - OPERATION TO MEMORY */
				insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_bits, ABSOLUTE, NO_OPERAND);
				/* END - OPERATION TO MEMORY */

				IC++;

				/* START - DESTINATION TO MEMORY */
				if(ptr -> dest_adr == immediate) {
					strcpy(ptr -> num_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == direct) {
					strcpy(ptr -> lbl_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, direct_operand, RELOCATABLE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == regis_direct) {
					strcpy(ptr -> num_to_mem, regis_to_num(ptr -> destination));
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, line_type_number, ABSOLUTE, DESTINATION_OP);
				}
				else if(ptr -> dest_adr == relative) {
					strcpy(ptr -> lbl_to_mem, ptr -> destination);
					insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, relative_operand, ABSOLUTE, DESTINATION_OP);
				}
				/* END - DESTINATION TO MEMORY */

				IC++;

				break;
			}

			case INSTRUCTION_WITHOUT_LABEL_NO_OPERANDS:  /* Result from line_parser: a line that contains only an operation */
			{
				/* START - OPERATION TO MEMORY */
				insert_memory_code_line(&head_mcl_ptr, &curr_mcl_ptr, IC, no_ops, ABSOLUTE, NO_OPERAND);
				/* END - OPERATION TO MEMORY */

				IC++;

				break;
			}

			case INVALID_LINE:  /* Result from line_parser: error was found during parsing */
			{
				error_controller(curr_line, ptr -> err, file_name);  /* add error */
				break;
			}

			case UNDEFINED_LINE:	/* something went wrong if reached here */
			{
				error_controller(curr_line, UNKNOWN_LINE, file_name);
				break;
			}
		}

		curr_line++;
	}


	/* update all DC values according to updated IC */	
	update_DC(head_mdl_ptr, IC);
	update_symb_table(IC);

	/* check if all entry labels were defined correctly */
	check_entry_labels();

	/* verify that all labels that were used as operands were defined correctly */
	second_pass_label_check(head_mcl_ptr, file_name);

	/* final check if no errors occurred before creating files */
	if(!were_errors()) {
		create_image_hexa_file(head_mcl_ptr, head_mdl_ptr, file_name, (IC-100), DC);
		create_ent_file(file_name);	/* a check if entry labels appeared is made inside this funtion */
		if(was_extern_label_as_operand(head_mcl_ptr)) {	/* make sure extern label/s appeared before creating externals file */
			create_extern_file(head_mcl_ptr, file_name);
		}
	}

	/* frees */
	free_code_image_nodes(&head_mcl_ptr);
	head_mcl_ptr = NULL;
	free_data_image_nodes(&head_mdl_ptr);
	head_mdl_ptr = NULL;
	free(ptr);	/* free line_details struct */
	ptr = NULL;

	if(!were_errors()) {
		return 1;
	}
	else return 0;
}


/**
 * find_next_line function finds where the new line starts at.
 *
 * @param fp - the pointer to the pointer which points to the file.
 */
void find_next_line(FILE** fp)
{
	char ch;

	while(1) {  /* Find the new line (or EOF) */
		ch = fgetc(*fp);
		if(ch == EOF || ch == '\n')
			break;
	}
}


/**
 * is_long_line function checks whether the line is too long. (MAX_LINE_LENGTH defined at 'line.details.h' and is equal to 81)
 *
 * @param fp - a pointer to the pointer which points to the file.
 * @return - 1 if line is too long, otherwise 0 is returned.
 */
int is_long_line(int line_len, int white_spaces_quantt)
{
	return (line_len - white_spaces_quantt > MAX_LINE_LENGTH) ? 1 : 0;
}


/**
 * is_line_ending_with_newline_char function checks whether the line is ending with a newline char.
 *
 * @param line - a pointer to the start of the line.
 * @param line_len - the length of the line.
 * @return - 1 if line is ending with newline char, otherwise 0 is returned.
 */
int is_line_ending_with_newline_char(char* line, int line_len)
{
	return line[line_len-1] == '\n' ? 1 : 0;
}


/**
 * get_op function returns the op enum value of the line currently processed in handler execution.
 * ops is an enum defined in 'line_details.h' file.
 *
 * @return - the enum value of the op currently processed in handler execution, if no op is processed, no_op (defined in the enum) is returned.
 */
ops get_op()
{
	return ptr -> en_op;
}


/**
 * get_dest_type function returns the addressing type of the destination operand currently processed in handler execution.
 * addressing_type is an enum defined in 'line_details.h' file.
 *
 * @return - the type of the destination operand currently processed in handler execution.
 */
addressing_type get_dest_type()
{
	return ptr -> dest_adr;
}


/**
 * get_src_type function returns the addressing type of the source operand currently processed in handler execution.
 * addressing_type is an enum defined in 'line_details.h' file.
 *
 * @return - the type of the source operand currently processed in handler execution.
 */
addressing_type get_src_type()
{
	return ptr -> src_adr;
}
char* get_number()
{
	return ptr -> num_to_mem;
}


/**
 * get_label function returns the label (label as source or destination operands) currently proccesed in handler execution.
 *
 * @return - the label currently processed in handler execution.
 */
char* get_label()
{
	return ptr -> lbl_to_mem;
}


/**
 * regis_to_num function returns the value of the register currently proccesed in handler execution.
 *
 * @param str - the string representation of the register.
 * @return - the string representation (in base 10) value of the register currently proccesed in handler execution.
 */
char* regis_to_num(char* str)
{
	static struct regis_binary {
		registers reg;
		char* reg_bin;
	} registers_binary[NUM_OF_REGISTERS] = {

	{r0, "1"}, 		/* 000000000001 in binary */
	{r1, "2"},		/* 000000000010 in binary */
	{r2, "4"},		/* 000000000100 in binary */
	{r3, "8"},		/* 000000001000 in binary */
	{r4, "16"},		/* 000000010000 in binary */
	{r5, "32"},		/* 000000100000 in binary */
	{r6, "64"},		/* 000001000000 in binary */
	{r7, "128"}		/* 000010000000 in binary */

	};

	/* variables definitions */
	int ind;
	char reg_num[2];

	strcpy(reg_num, str);
	reg_num[0] = reg_num[1];
	reg_num[1] = '\0';

	ind = atoi(reg_num);

	return registers_binary[ind].reg_bin;
}


/**
 * update_DC function updates all DC values to come after all IC values in memory image.
 *
 * @param node - a pointer to the head of the list of data memory image.
 * @param IC - all DC values are getting updated according to this IC.
 */
void update_DC(mdl_ptr node, int IC)
{
	while(node != NULL)
	{
		node -> DC = IC + (node -> DC);
		node = node -> next;
	}
}


/**
 * second_pass_label_check function checks whether all label used as operands were correctly defined.
 * The function does not return anything but if it finds any errors, it triggers an error edition which is checked later in the code to determine
 * next actions.
 *
 * @param node - a pointer to the head of the list of code memory image.
 * @param fn - the name of the file currently being checked.
 */
void second_pass_label_check(mcl_ptr node, const char* fn)
{
	int res;

	while(node != NULL)
	{
		if(node -> mlt == direct_operand) /* check if all labels used as direct operands were defined correctly */
		{
			res = symb_check(node -> l -> lbl);
			if(res != -1) {
				sprintf(node -> mem_val, "%d", res);
				if(is_extern_lbl(node -> l -> lbl)) {
					node -> memory_type = EXTERNAL;
				}
			}
			else {
				curr_line = node -> l -> lbl_line; /* update the line in which the label is in the original input file */
				if(node -> l -> op_type == SOURCE_OP) {
					error_controller(curr_line, UNDEFINED_LABEL_IN_SOURCE_OPERAND, fn);  /* add error */
				}
				else if(node -> l -> op_type == DESTINATION_OP) {
					error_controller(curr_line, UNDEFINED_LABEL_IN_DESTINATION_OPERAND, fn);  /* add error */
				}
			}
		}
		else if(node -> mlt == relative_operand) /* check if all labels used as relative operands were defined correctly */
		{
			res = symb_check(node -> l -> lbl);
			if(res != -1) {
				sprintf(node -> mem_val, "%d", (res - (node -> IC)));
			}
			else {
				curr_line = node -> l -> lbl_line; /* update the line in which the label is in the original input file */
				error_controller(curr_line, UNDEFINED_LABEL_IN_DESTINATION_OPERAND, fn);  /* add error */
			}
		}
		node = node -> next;
	}
}


/**
 * get_curr_line function returns the line number currently proccesed in handler execution.
 *
 * @return - the line number currently processed in handler execution.
 */
int get_curr_line()
{
	return curr_line;
}


/**
 * handler_free_mem function gets called by emergency_free_mem function (from main.c) to free all allocations made in 'handler.c' file.
 * This function is not included in any .header file ('only in emergency_free.mem_h') to prevent using it accidentally.
 */
void handler_free_mem()
{
	free_code_image_nodes(&head_mcl_ptr);
	head_mcl_ptr = NULL;
	free_data_image_nodes(&head_mdl_ptr);
	head_mdl_ptr = NULL;
	if(ptr != NULL) {
		free(ptr);	/* free line_details struct */
	}
	ptr = NULL;
}

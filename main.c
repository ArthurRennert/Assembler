/**
 * This is the main file and it contains access to "sensitive" pointers addresses and access to "sensitive" funtions (from 'errors.c', 'symbol_table.c').
 * 
 * @author - Arthur Rennert
 * Assembler Project
 */

#include "main.h"
#include "emergency_free_mem.h"


/* error list pointers */
err_ptr head_err = NULL;
err_ptr curr_err = NULL;

/* symbol table pointers */
symb_ptr head_symb = NULL;
symb_ptr curr_symb = NULL;

/* file name and file pointer variables */
char* fn = NULL;
FILE* fp = NULL;


/* main function */
int main(int argc, char* argv[])
{
	/* variable definitions */
	int handler_res;
	int i;

	/* variable initializations */
	handler_res = 0;
	i = 1;


	/* get files from terminal input loop */
	while(argv[i]) {
		fn = malloc(strlen(argv[i]) + 4); /* +3 for .as extension +1 for null string terminator */

		if(fn == NULL) {
			fprintf(stderr, "Fatal error: failed to allocate memory.\n");
			emergency_free_mem();
			exit(EXIT_FAILURE);
		}
		strcpy(fn, argv[i]);
		strcat(fn, AS_EXTENSION);
		fp = fopen(fn, "r");
		if(fp == NULL) {
			printf("File not found. Program terminated.\n");
			emergency_free_mem();
			exit(EXIT_FAILURE);
		}

		if(strlen(argv[i]) > MAX_FILE_NAME_LENGTH) {
			error_controller(0, FILE_NAME_LONGER_THAN_ALLOWED, fn);
		}

		handler_res = file_processor(&fp, fn);

		if(!handler_res || were_errors()) {
			print_errors(head_err);
		}
		else {
			putchar('\n');
			putchar('\n');
			printf("File '%s' has been assembled successfully!\n", fn);
		}

		fclose(fp);
		free(fn);
		fn = NULL;
		free_symb_list(&head_symb);
		head_symb = NULL;
		free_error_nodes(&head_err);
		head_err = NULL;
		i++;
	}

	putchar('\n');
	putchar('\n');
	puts("***********************************************************");
	puts("*\t\t\t\t\t\t\t  *");
	puts("*   Assembler execution has been finished successfully!   *");
	puts("*\t\t\t\t\t\t\t  *");
	puts("***********************************************************");
	putchar('\n');

	return EXIT_SUCCESS;
}


/**
 * error_controller function triggers 'error.c' file to create and insert an error to the errors list.
 *
 * @param ln - the line in the file where the error has occurred.
 * @param err - the enum value of the error. (enum ERR_VALUE is defined in 'line_details.h' file).
 * @param file_name - the name of the file in which the error has occured.
 */
void error_controller(int ln, ERR_VALUE err, const char* file_name)
{
	add_error(ln, err, &curr_err, &head_err, file_name);
}


/**
 * symb_controller function triggers 'symbol_table.c' file to create and insert an error to the symbol table list.
 *
 * @param lbl - the label needed to insert to the symbol list.
 * @param IC - the line code number where the label is defined.
 * @param att - the attribute of the label. (attribute enum is defined in 'line_details.h' file).
 */
void symb_controller(char* lbl, int IC, attribute att, ERR_VALUE* err)
{
    *err = NO_ERRORS;
	insert(lbl, &head_symb, &curr_symb, IC, att, err);
    
    if(*err != NO_ERRORS) {
        error_controller(get_curr_line(), *err, fn);
    }
}


/**
 * symb_check function searches if a label exists in the symbol table (list) and if the label exists, the function returns the address of the label.
 *
 * @param lbl - the label to search in the symbol table.
 * @return - the address of the label. If label was not found, -1 is returned.
 */
int symb_check(char* lbl)
{
	int res;

	res = second_pass_lbl(lbl, head_symb);
	if(res != -1) {
		return res;
	}
	else return -1;
}


/**
 * is_extern_lbl function checks whether the label is external. If the label is external, 1 is returned, otherwise 0 is returned.
 *
 * @param lbl - the label to check if it's an external label.
 * @return - 1 is returned if the label is external, otherwise 0 is returned.
 */
int is_extern_lbl(char* lbl)
{
	return is_extern_symb(lbl, head_symb);
}


/**
 * create_ent_file function creates an output entry file.
 * This function must be used ONLY if no errors were found during the execution of the 'assembler'.
 *
 * @param fn - the name of the file of which the output file is being created.
 */
void create_ent_file(const char* fn)
{
    if(was_entry_label_as_operand(head_symb)) {	/* make sure entry label/s appeared before creating entries file */
        create_entry_file(head_symb, fn);
    }
}


/**
 * update_symb_table function updates all '.data' and '.string' labels with the offset of IC (Instruction counter).
 *
 * @param IC - the total lines of instructions.
 */
void update_symb_table(int IC)
{
	symb_ptr node = NULL;

	node = head_symb;

	while(node != NULL)
	{
		if((node -> attr == data) || (node -> attr == data_entry)) {
			node -> addr = IC + (node -> addr);
		}
		node = node -> next;
	}
}


/**
 * were_errors function check whether errors were found during the 'assembler' execution.
 *
 * @return - 0 if no errors were found during the 'assembler' execution, otherwise return 1.
 */
int were_errors()
{
	if(head_err == NULL) {
		return 0;
	}
	else return 1;
}


/**
 * check_entry_labels  function checks whether all entry labels defined correctly.
 */
void check_entry_labels()
{
    were_all_entry_labels_defined(head_symb);
}


/**
 * get_file_name function returns the name of the file currently processed.
 *
 * @return - the name of the file currently processed.
 */
char* get_file_name()
{
    return fn;
}


/**
 * emergency_free_mem function frees all memory allocated during the execution. This function is called ONLY if any memory allocation fails.
 * It frees all the memory allocated until first memory allocation failure.
 * This function is not included in any .header file ('only in emergency_free.mem_h') to prevent using it accidentally.
 */
void emergency_free_mem()
{
	free_symb_list(&head_symb);
	head_symb = NULL;
	free_error_nodes(&head_err);
	head_err = NULL;
	if(fn != NULL) {
		free(fn);
	}
	fn = NULL;
	if(fp != 0) {
		fclose(fp);
	}
	handler_free_mem();
}

/**
 * This file contains all functions and information related to error handling in the 'assembler'.
 *
 * @author - Arthur Rennert
 * Assembler Project
 */

#include "errors.h"
#include "emergency_free_mem.h"

static int errors_in_how_many_files = 0;

/**
 * add_error function adds an error node to the errors-linked-list.
 * The function uses a static array of all possible errors of the assembler.
 *
 * @param ln - the line number in which the error has occurred.
 * @param err - the enum value of the error. (ERR_VALUE enum defined in 'line_details.h').
 * @param curr - the pointer to the pointer that points to the current error node.
 * @param head - the pointer to the pointer that points to the head error node.
 * @param fn - the name of the file in which the error has occurred.
 */
void add_error(int ln, ERR_VALUE err, err_ptr * curr, err_ptr * head, const char* fn)
{
   /* Array of all possible errors of the assembler */
	static struct error_msg {
		ERR_VALUE val;
		char* err_msg;
	} error_messages[NUM_OF_ERRORS] = {

	/* error_messages[0] */
	{LONG_LINE, "Line length is longer than allowed. Max line\nlength is 80 characters (not including the newline\ncharacter)."},

	/* error_messages[1] */
	{LABEL_BAD_SYNTAX, "Label syntax is wrong (only lowercase and\nuppercase letters and numbers are allowed.\nNumbers are not allowed in first letter of\nthe label)."},

	/* error_messages[2] */
	{LABEL_LENGTH_EXCEED, "Label length is longer than allowed. Max label\nlength is 31 letters."},

	/* error_messages[3] */
	{LABEL_LENGTH_EXCEED_AND_BAD_SYNTAX, "Label syntax is wrong and label is longer than\nallowed. Only lowercase and uppercase letters and\nnumbers are allowed. Numbers are not allowed in\nfirst letter of the label. Max label\nlength is 31 letters."},

	/* error_messages[4] */
	{LABEL_NAME_LIKE_OPERATION_NAME, "Label has same name like of an operation."},

	/* error_messages[5] */
	{LABEL_NAME_LIKE_REGISTER_NAME, "Label has same name like of a register."},

	/* error_messages[6] */
	{LABEL_NAME_LIKE_EXISTING_LABEL, "Label has same name like of a an existing label."},

	/* error_messages[7] */
	{NO_VALUES_AFTER_OPERATION, "Missing values after operation."},

	/* error_messages[8] */
	{TOO_MANY_VALUES_AFTER_OPERATION, "Too many values after operation."},

	/* error_messages[9] */
	{TOO_SHORT_STRING, "The string must be at least of size 2. A quatation\nmark at the begining and at the end. And any\nletters/numbers/symbols in between."},

	/* error_messages[10] */
	{STRING_STARTS_WITHOUT_QUATATION_MARK, "Quatation mark is missing in the beginning of the\nstring."},

	/* error_messages[11] */
	{STRING_ENDS_WITHOUT_QUATATION_MARK, "Quatation mark is missing in the end of the string."},

	/* error_messages[12] */
	{STRING_STARTS_AND_ENDS_WITHOUT_QUATATION_MARK, "Quatation mark is missing in the beginning and in\nthe end of the string."},

	/* error_messages[13] */
	{TOO_LONG_STRING, "The string is too long. Max string size is 1000\ncharacters."},

	/* error_messages[14] */
	{DATA_DOESNT_START_WITH_A_NUMBER, "Data doesn't start with an integer."},

	/* error_messages[15] */
	{WRONG_CHAR_IN_DATA, "Wrong char in data. Data must contain only\nintegers separetaed by commas. Data must start and\nend with integers. Whitespaces allowed between\ncommas and integers."},

	/* error_messages[16] */
	{MISSING_COMMA_IN_DATA, "A comma is missing. Data integers must be\nsepareted by commas."},

	/* error_messages[17] */
	{TOO_MANY_COMMAS_IN_DATA, "Data integers must be separeted only by one comma.\n A comma between every 2 integers."},

	/* error_messages[18] */
	{MISSING_OPERANDS, "Missing operand/s. 1 or 2 operands are missing,\naccording to the operation."},

	/* error_messages[19] */
	{TOO_MANY_OPERANDS, "Too many operands."},

	/* error_messages[20] */
	{INVALID_IMMEDIATE_OPERAND, "Invalid integer after #."},

	/* error_messages[21] */
	{MISSING_NUMBER_AFTER_NUMBER_SIGN, "An integer expected after #."},

	/* error_messages[22] */
	{BAD_SYNTAX_OF_SRC_OPERAND, "Invalid syntax of source operand."},

	/* error_messages[23] */
	{BAD_SYNTAX_OF_DEST_OPERAND, "Invalid syntax of destination operand."},

	/* error_messages[24] */
	{BAD_SYNTAX_OF_SRC_AND_DEST_OPERANDS, "Invalid syntax of both source and destination\noperands."},

	/* error_messages[25] */
	{MISSING_COMMA_BETWEEN_OPERANDS, "Missing comma between source and destination\noperands."},

	/* error_messages[26] */
	{A_REDUNDANT_COMMA_BETWEEN_OPERANDS, "A redundant comma between source and destination\noperands."},

	/* error_messages[27] */
	{INVALID_CHARACTER_BETWEEN_OPERANDS, "Invalid character in operands area, use a comma as\na separator between operands."},

	/* error_messages[28] */
	{REDUNDANT_INFORMATION_IN_OPERANDS_AREA, "Redundant information in operands area."},

	/* error_messages[29] */
	{BAD_OPERATION_SYNTAX, "Invalid operation syntax."},

	/* error_messages[30] */
	{MISSING_INFORMATION_AFTER_DATA, "Missing information after .data."},

	/* error_messages[31] */
	{MISSING_INFORMATION_AFTER_STRING, "Missing information after .string."},

	/* error_messages[32] */
	{UNKNOWN_LINE, "Unknown line."},

	/* error_messages[33] */
	{MISSING_INFORMATION_AFTER_LABEL, "Missing information after label."},

	/* error_messages[34] */
	{UNDEFINED_LABEL, "Undefined label."},

	/* error_messages[35] */
	{LABEL_NAME_LIKE_KEYWORD_NAME, "Label name like keyword name. Keywords: data,\nstring, entry, extern, PSW."},

	/* error_messages[36] */
	{UNDEFINED_LABEL_IN_DESTINATION_OPERAND, "Undefined label in destination operand."},

	/* error_messages[37] */
	{UNDEFINED_LABEL_IN_SOURCE_OPERAND, "Undefined label in source operand."},
        
	/* error_messages[38] */
	{ENTLY_LABEL_EXISTS_TRY_TO_ADD_EXTERN_LABEL, "Failed attempt to add an extern label. This label\nalready exists as entry label."},
        
	/* error_messages[39] */
	{EXTERN_LABEL_EXISTS_TRY_TO_ADD_ENTRY_LABEL, "Failed attempt to add an entry label. This label\nalready exists as extern label."},
        
	/* error_messages[40] */
	{LABEL_EXISTS_IN_CODE_TRY_TO_ADD_DATA_LABEL, "Failed attempt to add a data segment label. This\nlabel already exists as code segment label."},
        
	/* error_messages[41] */
	{LABEL_EXISTS_IN_DATA_TRY_TO_ADD_CODE_LABEL, "Failed attempt to add a code segment label. This\nlabel already exists as data segment label."},
        
	/* error_messages[42] */
	{DUPLICATE_CODE_LABEL_ATTEMPT, "Failed attempt to duplicate code segment label."},
        
	/* error_messages[43] */
	{DUPLICATE_DATA_LABEL_ATTEMPT, "Failed attempt to duplicate data degment label."},
        
	/* error_messages[44] */
	{UNDEFINED_ENTRY_LABEL, "Undefined entry label."},

	/* error_messages[45] */
	{FILE_NAME_LONGER_THAN_ALLOWED, "File name is longed than allowed.\nMax file name length allowed is 30 characters."},

	/* error_messages[46] */
	{DATA_DOESNT_END_WITH_A_NUMBER, "Data doesn't end with an integer."},
        
	/* error_messages[47 */
	{NO_ERRORS, "No errors."}

	};

	err_ptr p = malloc(sizeof(error_node));

   /* verify if malloc worked as expected */
	if(p == NULL)
    {
		fprintf(stderr, "Fatal error: failed to allocate memory.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	p -> str_error = malloc((strlen(error_messages[err].err_msg) + 1) * sizeof(char)); /* +1 for the string terminator char */

   /* verify if malloc worked as expected */
	if(p -> str_error == NULL)
    {
		free(p);
		fprintf(stderr, "Fatal error: failed to allocate memory.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	p -> file_name = malloc((strlen(fn) + 1) * sizeof(char)); /* +1 for the string terminator char */
    
   /* verify if malloc worked as expected */
	if(p -> file_name == NULL)
	{
		free(p -> str_error);
		free(p);
		fprintf(stderr, "Fatal error: failed to allocate memory.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	strcpy(p -> file_name, fn);

	/* if linked list is still empty */
	if(!(*head))
	{
		p -> line = ln;
		strcpy(p -> str_error, error_messages[err].err_msg);
		p -> next = NULL;
		*head = p;
		*curr = p;
		return;
	}
	else
	{
		/* if linked list is not empty */
		p -> line = ln;
		strcpy(p -> str_error, error_messages[err].err_msg);
		p -> next = NULL;
		(*curr) -> next = p;
		*curr = p;
	}
}


/**
 * print_errors function prints all errors that were found in the execution.
 *
 * @param head_node - the head of the errors-linked-list.
 */
void print_errors(err_ptr node)
{
	int err_ctr;
	err_ptr temp, head;
	errors_in_how_many_files++;
	temp = NULL;
	head = node;

	err_ctr = 0;
	temp = head;
	while(temp != NULL) {
		err_ctr++;
		temp = temp -> next;
	}

	putchar('\n');

	if(errors_in_how_many_files == 1) {
		puts("*******************************************************************");
		puts("*\t\t\t\t\t\t\t\t  *");
		puts("*   ATTENTION!!! If any line is invalid due to any reason *AND*   *\n*   the same line has a label definition, the label will *NOT*    *\n*    be added to the symbol table. And therefore the label will   *\n*    be UNDEFINED.\t\t\t\t\t\t  *");
		puts("*\t\t\t\t\t\t\t\t  *");
		puts("*******************************************************************");
	putchar('\n');
	putchar('\n');
	}

	printf("List of errors in file '%s' \n", head -> file_name);
	puts("-----------------------------------------------------------");

	if(err_ctr == 1) {
		puts("****  1 error detected  ****\n\n");
	}
	else {
		printf("****  %d Errors detected  ****\n\n\n", err_ctr);
	}

	temp = head;
	while(temp != NULL) {
		if(temp -> line == 0) {
			err_ctr--;
			printf("Error in file '%s':\n%s\n\n", temp -> file_name, temp -> str_error);
			temp -> line = -1;
			break;
		}
		temp = temp -> next;
	}


	while(err_ctr) {
		temp = head;

		while(temp != NULL && temp -> line == -1) {
			temp = temp -> next;
		}
		node = head;
		while(node != NULL) {
			if((node -> line != -1) && ((node -> line) < (temp -> line))) {
				temp = node;
			}
			node = node -> next;
		}
		printf("Error in line %d: %s\n\n", temp -> line, temp -> str_error);
		temp -> line = -1;
		err_ctr--;
	
	}

	printf("\n****  End of error list for file '%s'  ****\n", head -> file_name);
	puts("-----------------------------------------------------------");
}


/**
 * free_error_nodes function frees all error nodes that were created during the execution.
 *
 * @param head_node - the pointer to the pointer that points to the errors-linked-list.
 */
void free_error_nodes(err_ptr * head_node)
{
	err_ptr p;

	while(*head_node)
	{
		p = *head_node;
		*head_node = p -> next;
		free(p -> str_error);
		free(p -> file_name);
		free(p);
		p = NULL;
	}
}

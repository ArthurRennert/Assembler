/**
 * This file contains all functions and information related to symbol table handling in the 'assembler'.
 *
 * @author - Arthur Rennert
 * Assembler Project
 */

#include "symbol_table.h"
#include "emergency_free_mem.h"

/**
 * insert function creates a node and adds it to the symbol table (list), if list is still empty, it's being created upon first node creation.
 *
 * @param lbl - the label to add to the symbol table.
 * @param head - the pointer to the pointer that points to the head-symbol-table-linked-list.
 * @param curr - the pointer to the pointer that points to the current-symbol-table-linked-list.
 * @param IC - the number line in the file where the label is defined.
 * @param att - the attribute of the label. (attribute enum is defined in 'line_details.h').
 * @param err - a pointer to err. (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 */
void insert(char* lbl, symb_ptr * head, symb_ptr * curr, int IC, attribute att, ERR_VALUE* err)
{
	symb_ptr temp = NULL;
	symb_ptr p = NULL;

	if(search_lbl(lbl, *head, &temp)) { /* search if label already exists and update its attribute if needed */
		if((temp -> attr == entry && att == code) || (temp -> attr == code && att == entry)) {
			temp -> attr = code_entry;
		}
		else if((temp -> attr == entry && att == data) || (temp -> attr == data && att == entry)) {
			temp -> attr = data_entry;
			temp -> addr = IC;	/* in which IC the label has appeared */
		}
        else if((temp -> attr == entry || temp -> attr == code_entry || temp -> attr == data_entry) && att == external) {
            *err = ENTLY_LABEL_EXISTS_TRY_TO_ADD_EXTERN_LABEL;
            return;
        }
        else if(temp -> attr == external && att == entry) {
            *err = EXTERN_LABEL_EXISTS_TRY_TO_ADD_ENTRY_LABEL;
            return;
        }
        else if((temp -> attr == code || temp -> attr == code_entry) && att == data) {
            *err = LABEL_EXISTS_IN_CODE_TRY_TO_ADD_DATA_LABEL;
            return;
        }
        else if((temp -> attr == data || temp -> attr == data_entry) && att == code) {
            *err = LABEL_EXISTS_IN_DATA_TRY_TO_ADD_CODE_LABEL;
            return;
        }
        else if((temp -> attr == code || temp -> attr == code_entry) && att == code) {
            *err = DUPLICATE_CODE_LABEL_ATTEMPT;
            return;
        }
        else if((temp -> attr == data || temp -> attr == data_entry) && att == data) {
            *err = DUPLICATE_DATA_LABEL_ATTEMPT;
            return;
        }
        else if((temp -> attr == entry || temp -> attr == code_entry || temp -> attr == data_entry) && att == entry) {    /* It's ok to have multiple entry declarations of same label, but no need to add it again to the symbol table */
            return;
        }
        else if(temp -> attr == external && att == external) {  /* It's ok to have multiple extern declarations of same label, but no need to add it again to the symbol table */
            return;
        }
		return;
	}

	p = malloc(sizeof(symb));

	/* verify if malloc worked as expected */
	if(p == NULL)
	{
		fprintf(stderr, "Fatal error: failed to allocate memory.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}

	strcpy(p -> label, lbl);
	if(att == external) {
		p -> addr = 0;
	}
	else if(temp == NULL && att == entry) {
		p -> addr = get_curr_line();
	}
	else {
		p -> addr = IC;
	}
	p -> attr = att;
	p -> next = NULL;

	/* if linked list is still empty */
	if(!(*head))
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
 * search_lbl function searches if a label already exists in the symbol table (list).
 *
 * @param label - the label to search in the symbol table.
 * @param head - the pointer to the head-symbol-table-linked-list.
 * @param temp - the pointer to the pointer where to store the node address where the label is found (if found).
 * @return - 1 if label found in the symbol table, otherwise 0 is returned.
 */
int search_lbl(char* label, symb_ptr head, symb_ptr * temp)
{
	while(head != NULL)
	{
		if(strcmp(head -> label, label) == 0) {
			*temp = head;
			return 1;
		}

		head = head -> next;
	}
	return 0;
}


/**
 * second_pass_lbl function searches if a label exists in the symbol table (list) and if the label exists, the function returns the address of the label.
 *
 * @param label - the label to search in the symbol table.
 * @param head - the pointer to the head-symbol-table-linked-list.
 * @return - the address of the label. If label was not found, -1 is returned.
 */
int second_pass_lbl(char* label, symb_ptr head)
{
	while(head != NULL)
	{
		if(strcmp(head -> label, label) == 0) {
			return head -> addr;
		}

		head = head -> next;
	}
	return -1;
}


/**
 * is_extern_symb function checks whether the label is external. If the label is external, 1 is returned, otherwise 0 is returned.
 *
 * @param label - the label to check if it's an external label.
 * @param head - the pointer to the head-symbol-table-linked-list.
 * @return - 1 is returned if the label is external, otherwise 0 is returned.
 */
int is_extern_symb(char* label, symb_ptr head)
{
	while(head != NULL)
	{
		if(strcmp(head -> label, label) == 0) {
			return head -> attr == external ? 1 : 0;
		}
		head = head -> next;
	}
	return -1;
}


/**
 * create_entry_file function creates an output entry file.
 * This function must be used ONLY if no errors were found during the execution of the 'assembler'.
 *
 * @param head_symb - the pointer to the head-symbol-table-linked-list.
 * @param fn - the name of the file of which the output file is being created.
 */
void create_entry_file(symb_ptr node, const char* fn)
{
	char file_name[MAX_FILE_NAME_LENGTH + 5]; /* +1 for string terminator char, +4 for .ent extension */
	FILE* fp = NULL;


	strcpy(file_name, fn);	
	file_name[strlen(file_name) - 3] = '\0'; /* remove .as extension */
	strcat(file_name, ENT_EXTENSION);	/* add .ent extension */
	
	fp = fopen(file_name, "w+");

	if(fp == NULL) {
		printf("Could not create entries file.\n");
		emergency_free_mem();
		exit(EXIT_FAILURE);
	}


	while(node != NULL)
	{
		if(node -> attr == code_entry || node -> attr == data_entry)
		{
			fprintf(fp, "%s %04d\n", node -> label, node -> addr);
		}
		node = node -> next;
	}

	printf("\nFile '%s' has been created successfully!", file_name);

	fclose(fp);
}


/**
 * was_entry_label_as_operand function checks whether any entry label was used as an operand in an instruction.
 *
 * @param head_symb - the pointer to the head-symbol-table-linked-list.
 * @return - 1 If any entry label was used as an operand in an instruction, otherwise 0 is returned.
 */
int was_entry_label_as_operand(symb_ptr head_symb)
{
    while(head_symb != NULL)
    {
        if(head_symb -> attr == code_entry || head_symb -> attr == data_entry) {
            return 1;
        }
        head_symb = head_symb -> next;
    }
    return 0;
}


/**
 * were_all_entry_labels_defined function checks whether all entry labels defined correctly.
 *
 * @param head_symb - the pointer to the head-symbol-table-linked-list.
 * @return - 1 If all entry labels defined correctly, otherwise 0 is returned.
 */
void were_all_entry_labels_defined(symb_ptr head_symb)
{
    while(head_symb != NULL)
    {
        if(head_symb -> attr == entry) {
            error_controller(head_symb -> addr, UNDEFINED_ENTRY_LABEL, get_file_name());
        }
        head_symb = head_symb -> next;
    }
}


/**
 * free_symb_list function frees all symbol table (list) nodes that were created during the execution.
 *
 * @param head_node - the pointer to the pointer that points to the symbol-table-list.
 */
void free_symb_list(symb_ptr* head_node)
{
	symb_ptr p;

	while(*head_node != NULL)
	{
		p = *head_node;
		*head_node = p -> next;
		free(p);
		p = NULL;
	}
}

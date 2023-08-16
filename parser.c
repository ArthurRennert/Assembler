/**
 * This file contains all functions and information related to parsing handling in the 'assembler'.
 *
 * @author - Arthur Rennert
 * Assembler Project
 */

#include "parser.h"

/**
 * line_parser function parses the line and it returns it's type. see line-types in enum LINE_TYPE.
 *
 * @param line - the line to parse.
 * @param ld_ptr - the pointer to line_details struct. this function stores here all relevant information for 'handler.c'.
 * @return - LINE_TYPE enum value. 
 */
LINE_TYPE line_parser(char* line, line_details* ld_ptr)
{
	/* variables definitions */
	char* tokens[MAX_TOKENS];	/* all tokens of 'line' will be stored here for processing */
	char concat_str[MAX_STRING_SIZE];	/* used to concatenate data/string tokens for processing */
	int tkn_ctr;	/* tokens counter */
	int tkn_ind;   /* token index */
	int ops_num;	/* operands quantity found in line */
	int LINE_HAS_LABEL;	/* a flag that indicated if line has a label */
	char* data_tokens[MAX_DATA_TOKENS];	/* used to store here valid data tokens and pass them from here to 'handler.c' */
	int data_tkn_num;	/* the quantity of valid data tokens found */

	/* variables initializations */
	ops curr_op = no_op;
	tkn_ind = 0;
	tkn_ctr = 0;
	LINE_HAS_LABEL = 0;


	tkn_ctr = string_tokener(line, tokens);


	/* START - CHECK IF LINE IS AN EMPTY LINE */
	if(tkn_ctr == 1 && strlen(tokens[tkn_ind]) == 0) {
		return EMPTY;
	}
	/* END - CHECK IF LINE IS AN EMPTY LINE */


	/* START - CHECK IF LINE IS A COMMENT LINE */
	if(*(tokens[tkn_ind]) == ';') {
		return COMMENT;
	}
	/* END - CHECK IF LINE IS A COMMENT LINE */


	/* START - CHECK FOR POTENTIAL LABEL */
	if(is_potential_label(tokens[tkn_ind])) {  /* tkn_ind = 0 at this point */
		LINE_HAS_LABEL = 1;
		if(is_label(tokens[tkn_ind], &(ld_ptr -> err))) {
			strcpy(ld_ptr -> label, tokens[tkn_ind]);
		}
		else return INVALID_LINE;	/* INVALID LABEL */
		tkn_ind++;
		if(!(tkn_ctr-tkn_ind)) { 	/* is there are still tokens to parse */
			ld_ptr -> err = MISSING_INFORMATION_AFTER_LABEL;
			return INVALID_LINE;
		}
	}
	/* END - CHECK FOR POTENTIAL LABEL */


	/* START - CHECK IF INSTRUCTION LINE */
	curr_op = is_operation(tokens[tkn_ind]);
	if(curr_op != no_op) {
		strcpy(ld_ptr -> op, tokens[tkn_ind]);
		ld_ptr -> en_op = curr_op;
		tkn_ind++;


	/* START - IF INSTRUCTION LINE, CHECK OPERANDS' VALIDITY AND ADDRESSING TYPES */
		ops_num = tkn_ctr - tkn_ind; /* THIS ops_num MAY NOT BE TRUE, THE TRUE VALUE RETURNED BY operands_parser function */
/* printf("ops_num is: %d\n", ops_num); */
		if(ops_num == 2) {
			ops_num = operands_parser(ld_ptr -> source, ld_ptr -> destination, &(ld_ptr -> src_adr), &(ld_ptr -> dest_adr), &(ld_ptr -> err), ops_num, tokens[tkn_ind], tokens[tkn_ind+1]);
		}
		else if(ops_num == 1) {
			ops_num = operands_parser(ld_ptr -> source, ld_ptr -> destination, &(ld_ptr -> src_adr), &(ld_ptr -> dest_adr), &(ld_ptr -> err), ops_num, tokens[tkn_ind]);
		}
		else if(ops_num == 3) {
			ops_num = operands_parser(ld_ptr -> source, ld_ptr -> destination, &(ld_ptr -> src_adr), &(ld_ptr -> dest_adr), &(ld_ptr -> err), ops_num, tokens[tkn_ind], tokens[tkn_ind+1], tokens[tkn_ind+2]);
		}
		else if(ops_num >= 4) {
			ld_ptr -> err = REDUNDANT_INFORMATION_IN_OPERANDS_AREA;
			return INVALID_LINE;
		}
	/* END - IF INSTRUCTION LINE, CHECK OPERANDS' VALIDITY AND ADDRESSING TYPES */


		/* START - check if operands_parser function confirms no error before moving to next step of parsing */
		if(ld_ptr -> err != NO_ERRORS) {
			return INVALID_LINE;
		}
		/* END - check if operands_parser function confirms no error before moving to next step of parsing */


		/* START - CHECK IF OPERATION MATCHES OPERANDS */
		if(is_op_operands_match(curr_op, &(ld_ptr -> src_adr), &(ld_ptr -> dest_adr), ops_num, &(ld_ptr -> err))) 			{
			if(ops_num == 2) {
				if(LINE_HAS_LABEL) {
					return INSTRUCTION_WITH_LABEL_2_OPERANDS;
				}
				else {
					return INSTRUCTION_WITHOUT_LABEL_2_OPERANDS;
				}
			}
			else if(ops_num == 1) {
				if(LINE_HAS_LABEL) {
					return INSTRUCTION_WITH_LABEL_1_OPERAND;
				}
				else {
					return INSTRUCTION_WITHOUT_LABEL_1_OPERAND;
				}
			}
			else if(ops_num == 0) {
				if(LINE_HAS_LABEL) {
					return INSTRUCTION_WITH_LABEL_NO_OPERANDS;
				}
				else {
					return INSTRUCTION_WITHOUT_LABEL_NO_OPERANDS;
				}
			}
		}
		else {
			return INVALID_LINE;		/* is_op_operands_match function assigned the corresponding error-enum */
		}
		/* END - CHECK IF OPERATION MATCHES OPERANDS */


	}
	/* END - CHECK IF INSTRUCTION LINE */


	/* START - IF LINE IS GUIDANCE ENTRY LINE */
	if(is_entry(tokens[tkn_ind])) {
		if(is_label(tokens[tkn_ind+1], &(ld_ptr -> err))) {
			strcpy(ld_ptr -> label, tokens[tkn_ind+1]);
			return ENTRY;
		}
		else {
			return INVALID_LINE;
		}
	}
	/* END - IF LINE IS GUIDANCE ENTRY LINE */


	/* START - IF LINE IS GUIDANCE EXTERN LINE */
	else if(is_extern(tokens[tkn_ind])) {
		if(is_label(tokens[tkn_ind+1], &(ld_ptr -> err))) {
			strcpy(ld_ptr -> label, tokens[tkn_ind+1]);
			return EXTERN;
		}
		else {
			return INVALID_LINE;
		}
	}
	/* END - IF LINE IS GUIDANCE EXTERN LINE */


	/* START - IF LINE IS GUIDANCE STRING LINE */
	else if(is_string_line(tokens[tkn_ind])) {
		tkn_ind++;
		if(concat_string_tokens(tkn_ind, tkn_ctr-1, tokens, concat_str, &(ld_ptr -> err))) {
			if(is_string(concat_str, &(ld_ptr -> err))) {
				remove_string_marks(strlen(concat_str), concat_str);
				strcpy(ld_ptr -> string, concat_str);
				if(LINE_HAS_LABEL) {
					return GUIDANCE_STRING_WITH_LABEL;
				}
				else return GUIDANCE_STRING_WITHOUT_LABEL;
			}
			else return INVALID_LINE;
		}
		else return INVALID_LINE;
	}
	/* END - IF LINE IS GUIDANCE STRING LINE */


	/* START - IF LINE IS GUIDANCE DATA LINE */
	else if(is_data_line(tokens[tkn_ind])) {
		if(!concat_data_tokens(tkn_ind+1, tkn_ctr-1, tokens, concat_str, &(ld_ptr -> err))) {
			return INVALID_LINE;
		}
		if(is_data(concat_str, &(ld_ptr -> err))) {
			split_data(concat_str, data_tokens, &data_tkn_num);

		copy_data(data_tkn_num, ld_ptr -> data, data_tokens);
		ld_ptr -> num_in_data_array = data_tkn_num;
		if(LINE_HAS_LABEL) {
			return GUIDANCE_DATA_WITH_LABEL;
		}
		else return GUIDANCE_DATA_WITHOUT_LABEL;
		}
		else return INVALID_LINE;
	}
	/* END - IF LINE IS GUIDANCE DATA LINE */

	ld_ptr -> err = UNKNOWN_LINE;	/* if reached here, something went wrong */
	return INVALID_LINE;
}


/**
 * which_addr_type function returns the addressing type enum value of the operand. See addressing_type enum definition in 'line_details.h'.
 *
 * @param str - the string of the potential operand.
 * @param err - a pointer to err. (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - addressing_type enum value. 
 */
addressing_type which_addr_type(char* str, ERR_VALUE* err)
{
	/* variables definitions */
	int len;
	int i;

	/* variables initializations */
	len = strlen(str);
	i = 0;


	/* START - immediate */
	if(str[i] == '#') {
		i++;
		if(len > 1) {
			if(str[i] == '-' && len != 2) {
				i++;
			}
			while(i < len) {
				if(!isdigit(str[i])) {
					*err = INVALID_IMMEDIATE_OPERAND;
					return no_type;
				}

			remove_first_char(len, str);
			return immediate;
			}
		}
		else {
			*err = MISSING_NUMBER_AFTER_NUMBER_SIGN;
			return no_type;
		}
	}
	/* END - immediate */


	/* START - register direct */
	else if(is_register(str))
		return regis_direct;
	/* END - register direct */


	/* START - relative */
	else if(str[i] == '%') { /* i still 0 at this point */

		/* remove the '%' to check if it's a label */
		remove_first_char(len, str);
		/* END - remove the '%' to check if it's a label */

		if(is_label(str, err)) {
			return relative;  /* keep parsing like its valid relative - check validity will be checked second pass */
		}
	}
	/* END -relative */


	/* START - direct */
	else if (is_label(str, err)) {
		return direct;  /* keep parsing like its valid direct */
	}
	/* END - direct */

	return no_type;  /* if reached here, operand is invalid */
}


/**
 * count_whitespaces function counts the whitespaces in 'str' string and returns the it's quantity.
 *
 * @param str - the string in which whitespaces will be counted.
 * @return - the quantity of whitespaces detected. 
 */
int count_whitespaces(const char* str)
{
	int ctr;
	int i;
	int str_len;
	ctr = 0;
	str_len = strlen(str);

	for(i = 0; i < str_len; i++) {
		if(str[i] == ' ' || str[i] == '\t') {
			ctr++;
		}	
	}
	return ctr;
}


/**
 * is_label function checks if 'str' is a valid label.
 *
 * @param str - the potential label.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if label is valid, otherwise 0 is returned. 
 */
int is_label(char* str, ERR_VALUE* err)
{
	int i, len;

	len = strlen(str);

	if(len > MAX_OPERAND_AND_LABEL_LENGTH) {
		*err = LABEL_LENGTH_EXCEED;
	}

	if(!((str[0] >= A && str[0] <= Z) || (str[0] >= a && str[0] <= z))) { /* label doesn't start with a letter */
		if(*err == LABEL_LENGTH_EXCEED) {
			*err = LABEL_LENGTH_EXCEED_AND_BAD_SYNTAX;
			return 0;
		}
		else {
			*err = LABEL_BAD_SYNTAX;
			return 0;
		}
		
	}

	for(i = 1; i < len; i++) {  /* check the rest (first letter checked) letters of the potential label */
		if(!((str[i] >= A && str[i] <= Z) || (str[i] >= a && str[i] <= z) || (str[i] >= zero && str[i] <= nine))) {
			if(*err == NO_ERRORS) {
				*err = LABEL_BAD_SYNTAX;
				return 0;
			}
			else if(*err == LABEL_LENGTH_EXCEED) {
				*err = LABEL_LENGTH_EXCEED_AND_BAD_SYNTAX;
				return 0;
			}
		}
	}
	
	/* if label syntax is bad or label length is too long than no chance for the (not valid) label to be like operation/existing (valid) label or like a register*/
	if((*err == NO_ERRORS)) {

		/* check if the label is like one of the registers names */
		if(is_register(str)) {
			*err = LABEL_NAME_LIKE_REGISTER_NAME;
			return 0;
		}

		/* check if label is like one of the keywords names */
		if(is_keyword(str)) {
			*err = LABEL_NAME_LIKE_KEYWORD_NAME;
			return 0;
		}
		/* check if the label is like one of the operations names */
		if(is_operation(str) != no_op) {
			*err = LABEL_NAME_LIKE_OPERATION_NAME;
			return 0;
		}

	}
	return 1;
}


/**
 * is_data function checks if all string after .data is a valid.
 *
 * @param str - the potential array of numbers.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if array values are valid, otherwise 0 is returned. 
 */
int is_data(char* str, ERR_VALUE* err)
{
	int len;
	int i = 0;
	len = strlen(str);


	if(!isdigit(str[0]) && str[0] != '-' && str[0] != '+') {
		*err = DATA_DOESNT_START_WITH_A_NUMBER;
		return 0;
	}


	if(!isdigit(str[strlen(str) - 1])) {
		*err = DATA_DOESNT_END_WITH_A_NUMBER;
		return 0;
	}

	while(str[i] != '\0') {
		if(isdigit(str[i]) || str[i] == '-' || str[i] == '+') {
			if((str[i] == '-' || str[i] == '+') && (!isdigit(str[i+1]))) {
				*err = WRONG_CHAR_IN_DATA;
				return 0;
			}

			i++;
			while((i < len) && isdigit(str[i])) i++;  /* skip while there's a comma */
			if(i == len) /* end of data */
				return 1;
			else if(str[i] == ',') {
				i++;
				continue;
			}
			else {
				*err = WRONG_CHAR_IN_DATA;
				return 0;
			}
		}
		else {
			*err = WRONG_CHAR_IN_DATA;
			return 0;
		}
	}
	return 1;
}


/**
 * is_string function checks if 'str' is a valid string. 'str' is what comes after .string.
 * In case the function detected a non valid label, the corresponding err enum value is set to err param.
 *
 * @param str - the potential string.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if string is valid, otherwise 0 is returned. 
 */
int is_string(char* str, ERR_VALUE* err)
{
	int len = strlen(str);

	if(len == 1) {
		*err = TOO_SHORT_STRING;
		return 0;
	}

	if((len == 2) && (str[0] == '"' && str[1] == '"')) {  /* an empty string */
		return 1;
	}

	if(str[0] != '"' && str[len-1] == '"') {
		*err = STRING_STARTS_WITHOUT_QUATATION_MARK;
		return 0;
	}

	if(str[0] == '"' && str[len-1] != '"') {
		*err = STRING_ENDS_WITHOUT_QUATATION_MARK;
		return 0;
	}

	if(str[0] != '"' && str[len-1] != '"') {
		*err = STRING_STARTS_AND_ENDS_WITHOUT_QUATATION_MARK;		
		return 0;
	}

	if(len > MAX_STRING_SIZE + 2) { /* 2 for the quatation marks */
		*err = TOO_LONG_STRING;
		return 0;
	}

	/* if reached here, string is valid */
	return 1;
}


/**
 * is_operation function checks 'str' is a valid operation. See ops enum definition in 'line_details.h' file.
 *
 * @param str - the string to check if it's an operation.
 * @return - ops enum value. no_op (ops enum value) is returned in case the function detected a non operation 'str'.
 */
ops is_operation(char* str)  /* ops is an enum defined in line_details.h */
{
	
	static const char* operations[NUM_OF_OPERATIONS] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};

	int i;

	for(i = 0; i < NUM_OF_OPERATIONS; i++)
		if(strcmp(str, operations[i]) == 0)
			return i;

	return no_op;
}


/**
 * is_register function checks whether 'str' is a one of the registers.
 *
 * @param str - the string to check if it's a register.
 * @return - 1 if 'str' is one of the registers, otherwise 0 is returned.
 */
int is_register(char* str)
{
	static const char* registers[NUM_OF_REGISTERS] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

	int i;

	for(i = 0; i < NUM_OF_REGISTERS; i++)
		if(strcmp(str, registers[i]) == 0)
			return 1;

	return 0;
}


/**
 * is_keyword function checks whether 'str' is a one of the keywords.
 *
 * @param str - the string to check if it's a keyword.
 * @return - 1 if 'str' is one of the registers, otherwise 0 is returned.
 */
int is_keyword(char* str)
{
	static const char* keywords[NUM_OF_KEYWORDS] = {"PSW", "data", "string", "entry", "extern"};

	int i;

	for(i = 0; i < NUM_OF_KEYWORDS; i++) {
		if(strcmp(str, keywords[i]) == 0) {
			return 1;
		}
	}

	return 0;
}


/**
 * string_tokener function gets a line and an array of strings and it breakes the line into tokens and stores the tokens in the array.
 *
 * @param line - the line to break into tokens.
 * @param tokens - the array of strings to store the tokens.
 * @return - the total number of tokens that the function found.
 */
int string_tokener(char* line, char** tokens)
{
	static const char line_delimit[] = " \t";
	char* token;
	int tkn_ctr;

	token = strtok(line, line_delimit);
	tkn_ctr = 0;

	while(token != NULL) {
		tokens[tkn_ctr] = token;
		tkn_ctr++;
		token = strtok(NULL, line_delimit);
	}

	tokens[tkn_ctr-1][strlen(tokens[tkn_ctr-1])-1] = '\0';  /* last char in line is always /n (newline char), we replace it with string null terminator */
	return tkn_ctr;
}


/**
 * is_potential_label function returns whether the label has ':' in the end of it. If it has ':' in the end of it, it replaces it with the string null 
 * terminator char and returns 1;
 *
 * @param lbl - the label to check.
 * @return - 1 if 'lbl' string has the char ':' in the end of it, otherwise 0 is returned.
 */
int is_potential_label(char* lbl)
{	
	if(lbl[strlen(lbl)-1] == ':') {
		lbl[strlen(lbl)-1] = '\0'; /* remove the colon for convinient label handling */
		return 1;
	}
	return 0;
}


/**
 * operands_parser function gets 1, 2 or 3 tokens and it checks whether the tokens are some combination of valid operands.
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
int operands_parser(char* src, char* dest, addressing_type* src_type, addressing_type* dest_type, ERR_VALUE* err, int ops_num, ...)
{
	/* variables definitions */
	int ops_quantt;	/* operations quantity */
	char op1[MAX_OPERAND_AND_LABEL_LENGTH + 1];  /* +1 for the null terminating character in case label length is max (31 characters) */
	char op2[MAX_OPERAND_AND_LABEL_LENGTH + 1];	/* +1 for the null terminating character in case label length is max (31 characters) */
	char buffer[MAX_OPERAND_AND_LABEL_LENGTH * 2];
	addressing_type op1_type;
	addressing_type op2_type;
	int i;	/* loop counter */
	int j;	/* loop counter */
	int len;
	va_list p;

	/* variables initializations */
	op1_type = no_type;
	op2_type = no_type;
	ops_quantt = ops_num;
	va_start(p, ops_num);

	switch(ops_quantt) {
		case 1: {
			strcpy(buffer, va_arg(p, char*));
			len = strlen(buffer);
			for(i = 0; i < len; i++) {
				if(buffer[i] == ',') {
					break;
				}
			}
			if(i < len - 1) {	/* a possibility of 2 operands */
				for(j = 0; j < i; j++) {
					op1[j] = buffer[j];
				}
				op1[j] = '\0';
				for(j = j + 1; j < len; j++) {  /* from j + 1 to skip the comma */
					op2[j-i-1] = buffer[j];
				}
				op2[j-i-1] = '\0';
				break;
			}
			else if(i == len) {	/* a possibility of 1 operand */
				op1_type = which_addr_type(buffer, err);
				if(op1_type == no_type) {
					*err = BAD_SYNTAX_OF_DEST_OPERAND;
					va_end(p);
					return 0;
				}
				else {
					*(dest_type) = op1_type;
					strcpy(dest, buffer);
					va_end(p);
					return 1;
				}
			}
			break;
		}
		case 2: {
			strcpy(op1, va_arg(p, char*));
			strcpy(op2, va_arg(p, char*));
			if(op1[strlen(op1) - 1] != ',' && op2[0] != ',') {
				*err = MISSING_COMMA_BETWEEN_OPERANDS;
				va_end(p);
				return 0;
			}
			else if(op1[strlen(op1) - 1] == ',' && op2[0] == ',') {
				*err = A_REDUNDANT_COMMA_BETWEEN_OPERANDS;
				va_end(p);
				return 0;
			}
			else if(op1[strlen(op1) - 1] == ',') {
				op1[strlen(op1) - 1] = '\0';
				break;
			}
			else if(op2[0] == ',') {
				for(i = 0; i < strlen(op2)-2; i++) {
					op2[i] = op2[i+1];
				}
				op2[i] = '\0';
			}
			break;
		}
		case 3: {
			strcpy(op1, va_arg(p, char*));
			strcpy(buffer, va_arg(p, char*));
			strcpy(op2, va_arg(p, char*));
			if(!(*buffer == ',' && strlen(buffer) == 1)) {
				*err = INVALID_CHARACTER_BETWEEN_OPERANDS;
				va_end(p);
				return 0;
			}
			break;
		}
	}
		op1_type = which_addr_type(op1, err);
		op2_type = which_addr_type(op2, err);

		if(op1_type != no_type && op2_type != no_type) {
			*(src_type) = op1_type;
			*(dest_type) = op2_type;
			strcpy(src, op1);
			strcpy(dest, op2);
			va_end(p);
			return 2;
		}
		if(op1_type == no_type && op2_type == no_type) {
			*err = BAD_SYNTAX_OF_SRC_AND_DEST_OPERANDS;
		}
		else if(op1_type == no_type) {
			*err = BAD_SYNTAX_OF_SRC_OPERAND;
		}
		else if(op2_type == no_type) {
			*err = BAD_SYNTAX_OF_DEST_OPERAND;
		}

	va_end(p);
	return 0;
}


/**
 * is_op_operands_match function checks whether the op and operands match.
 *
 * @param op - the enum value of the operand. See ops enum definition in 'line_details.h' file.
 * @param src_adr - the addressing type of the source operand. See addressing_type enum definition in 'line_details.h' file.
 * @param dest_adr - the addressing type of the destination operand. See addressing_type enum definition in 'line_details.h' file.
 * @param ops_num - the number of tokens being sent to the function.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 is returned if the op with the operand/s do match, otherwise 0 is returned.
 */
int is_op_operands_match(ops op, addressing_type* src_adr, addressing_type* dest_adr, int ops_num, ERR_VALUE* err)
{
	switch(op) {
		case mov:
		{
			if(ops_num == 2) {
				if(*src_adr == immediate || *src_adr == direct || *src_adr == regis_direct) {
					if(*dest_adr == direct || *dest_adr == regis_direct) {
						return 1;
					}
					else {
						*err = BAD_SYNTAX_OF_DEST_OPERAND;
						return 0;
					}
				}
				else {
					*err = BAD_SYNTAX_OF_SRC_OPERAND;
					return 0;
				}
			}
			else {
				*err = MISSING_OPERANDS;  /* ops_num = 1 */
				return 0;
			}
			break;
		}
		case cmp:
		{
			if(ops_num == 2) {
				if(*src_adr == immediate || *src_adr == direct || *src_adr == regis_direct) {
					if(*dest_adr == immediate || *dest_adr == direct || *dest_adr == regis_direct) {
						return 1;
					}
					else {
						*err = BAD_SYNTAX_OF_DEST_OPERAND;
						return 0;
					}
				}
				else {
					*err = BAD_SYNTAX_OF_SRC_OPERAND;
					return 0;
				}
			}
			else {
				*err = MISSING_OPERANDS;  /* ops_num = 1 */
				return 0;
			}
			break;
		}
		case add: /* same like case sub */
		case sub:
		{
			if(ops_num == 2) {
				if(*src_adr == immediate || *src_adr == direct || *src_adr == regis_direct) {
					if(*dest_adr == direct || *dest_adr == regis_direct) {
						return 1;
					}
					else {
						*err = BAD_SYNTAX_OF_DEST_OPERAND;
						return 0;
					}
				}
				else {
					*err = BAD_SYNTAX_OF_SRC_OPERAND;
					return 0;
				}
			}
			else {
				*err = MISSING_OPERANDS;  /* ops_num = 1 */
				return 0;
			}
			break;
		}
		case lea:
		{
			if(ops_num == 2) {
				if(*src_adr == direct) {
					if(*dest_adr == direct || *dest_adr == regis_direct) {
						return 1;
					}
					else {
						*err = BAD_SYNTAX_OF_DEST_OPERAND;
						return 0;
					}
				}
				else {
					*err = BAD_SYNTAX_OF_SRC_OPERAND;
					return 0;
				}
			}
			else {
				*err = MISSING_OPERANDS;  /* ops_num = 1 */
				return 0;
			}
			break;
		}
		case clr:  /* like case dec */
		case not:  /* like case dec */
		case inc:  /* like case dec */
		case dec:
		{
			if(ops_num == 1) {
				if(*dest_adr == direct || *dest_adr == regis_direct) {
					return 1;
				}
				else {
					*err = BAD_SYNTAX_OF_DEST_OPERAND;
					return 0;
				}
			}
			else {
				*err = TOO_MANY_OPERANDS;  /* ops_num = 2 */
				return 0;
			}
			break;
		}
		case jmp:  /* like case jsr */
		case bne:  /* like case jsr */
		case jsr:
		{
			if(ops_num == 1) {
				if(*dest_adr == direct || *dest_adr == relative) {
					return 1;
				}
				else {
					*err = BAD_SYNTAX_OF_DEST_OPERAND;
					return 0;
				}
			}
			else {
				*err = TOO_MANY_OPERANDS;  /* ops_num = 2 */
				return 0;
			}
			break;
		}
		case red:
		{
			if(ops_num == 1) {
				if(*dest_adr == direct || *dest_adr == regis_direct) {
					return 1;
				}
				else {
					*err = BAD_SYNTAX_OF_DEST_OPERAND;
					return 0;
				}
			}
			else {
				*err = TOO_MANY_OPERANDS;  /* ops_num = 2 */
				return 0;
			}
			break;
		}
		case prn:
		{
			if(ops_num == 1) {
				if(*dest_adr == immediate || *dest_adr == direct || *dest_adr == regis_direct) {
					return 1;
				}
				else {
					*err = BAD_SYNTAX_OF_DEST_OPERAND;
					return 0;
				}
			}
			else {
				*err = TOO_MANY_OPERANDS;  /* ops_num = 2 */
				return 0;
			}
			break;
		}
		case rts:  /* like case stop */
		case stop:
		{
			if(ops_num) {
				*err = REDUNDANT_INFORMATION_IN_OPERANDS_AREA;
				return 0;
			}
			else return 1;
			break;
		}
		case no_op:
		{
			*err = BAD_OPERATION_SYNTAX;
			return 0;
		}
	}

	*err = REDUNDANT_INFORMATION_IN_OPERANDS_AREA;
	return 0;
}


/**
 * is_entry function checks whether 'str' is ".entry".
 *
 * @param str - the string to check if it's ".entry".
 * @return - 1 is returned if 'str' is ".entry", otherwise 0 is returned.
 */
int is_entry(char* str)
{
	return strcmp(str, ENTRY_STR) == 0 ? 1 : 0;
}


/**
 * is_extern function checks whether 'str' is ".extern".
 *
 * @param str - the string to check if it's ".extern".
 * @return - 1 is returned if 'str' is ".extern", otherwise 0 is returned.
 */
int is_extern(char* str)
{
	return strcmp(str, EXTERN_STR) == 0 ? 1 : 0;
}


/**
 * is_data_line function checks whether 'str' is ".data".
 *
 * @param str - the string to check if it's ".data".
 * @return - 1 is returned if 'str' is ".data", otherwise 0 is returned.
 */
int is_data_line(char* str)
{
	return strcmp(str, DATA_STR) == 0 ? 1 : 0;
}


/**
 * is_string_line function checks whether 'str' is ".string".
 *
 * @param str - the string to check if it's ".string".
 * @return - 1 is returned if 'str' is ".string", otherwise 0 is returned.
 */
int is_string_line(char* str)
{
	return strcmp(str, STRING_STR) == 0 ? 1 : 0;
}


/**
 * concat_data_tokens function concatenating data tokens into one token.
 *
 * @param from - the index of the first data token in the array of tokens.
 * @param to - the index of the last data token in the array of tokens.
 * @param tkn_ptr - the array of tokens (with the data tokens in it).
 * @param str - the string where to concatenate all the data tokens to.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 is returned if 'str' is ".string", otherwise 0 is returned.
 */
int concat_data_tokens(int from, int to, char** tkn_ptr, char* concat_str, ERR_VALUE* err)
{
	if(from <= to) {
		strcpy(concat_str, tkn_ptr[from]);
		from++;
	}
	else {
		*err = MISSING_INFORMATION_AFTER_DATA;
		return 0;
	}
		
	while(from <= to) {
		if(concat_str[strlen(concat_str)-1] != ',' && tkn_ptr[from][0] != ',') {
			*err = MISSING_COMMA_IN_DATA;
			return 0;
		}
		else if (concat_str[strlen(concat_str)-1] == ',' && tkn_ptr[from][0] == ',') {
			*err = TOO_MANY_COMMAS_IN_DATA;
			return 0;
		}
		/* if reached here, only one of the tokens has a comma */
		strcat(concat_str, tkn_ptr[from]);
		from++;
	}
	return 1;
}


/**
 * concat_string_tokens function concatenating string tokens into one token.
 *
 * @param from - the index of the first string token in the array of tokens.
 * @param num - the quantity of string tokens in they array of tokens.
 * @param tkn_ptr - the array of tokens (with the string tokens in it).
 * @param str - the string where to concatenate all the string tokens to.
 * @param err - a pointer to err (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 * @return - 1 if all data tokens were valid, otherwise 0 is returned.
 */
int concat_string_tokens(int from, int num, char** tkn_ptr, char* concat_str, ERR_VALUE* err)
{
	static const char space[] = " ";

	if(from <= num) {
		strcpy(concat_str, tkn_ptr[from]);
		strcat(concat_str, space);
		from++;
	}
	else {
		*err = MISSING_INFORMATION_AFTER_STRING;
		return 0;
	}

	while(from <= num) {
		strcat(concat_str, tkn_ptr[from]);
		strcat(concat_str, space);
		from++;
	}
	concat_str[strlen(concat_str)-1] = '\0';
	return 1;
}


/**
 * split_data function splits the 'data_token' into many data tokens.
 *
 * @param str - the token to break to many data tokens.
 * @param data_tokens - an array of strings where to store the data tokens. (later on atoi function will convert it to integers).
 * @param num - an address of an integer where to store the number of data tokens stored.
 */
void split_data(char* concat_str, char** data_tokens, int* num)
{
	static const char comma_delimit[] = ",";
	char* data_token;
	int data_tkn_ind;

	data_token = strtok(concat_str, comma_delimit);
	data_tkn_ind = 0;

	while(data_token != NULL) {
		data_tokens[data_tkn_ind] = data_token;
		data_tkn_ind++;
		data_token = strtok(NULL, comma_delimit);
	}

	*num = data_tkn_ind;
}


/**
 * copy_data function converts data tokens to integers and copies them to 'arr' array.
 *
 * @param num - the number of data tokens to be converted to integers and copied to 'arr' array.
 * @param arr - copy the data tokens to this array.
 * @param data_tokens - an array of strings where the data tokens are stored.
 */
void copy_data(int num, int* arr, char** data_tokens)
{
	int i;
		for(i = 0; i < num; i++) {
		arr[i] = atoi(data_tokens[i]);
	}
}


/**
 * remove_string_marks function removes the quatation marks from the begining and the end of the 'concat_str' string.
 *
 * @param len - the length of 'concat_str' string.
 * @param concat_str - quatation marks will be removed from this string.
 */
void remove_string_marks(int len, char* concat_str)
{
	int i;

	for(i = 0; i < len; i++) {
		concat_str[i] = concat_str[i+1];
	}

	concat_str[len-2] = '\0';
}


/**
 * remove_first_char function removes the removes the first char in 'str' string.
 *
 * @param len - the length of 'str' string.
 * @param str - the first char of this string will be removed.
 */
void remove_first_char(int len, char* str)
{
	int i;

	for(i = 0; i < len; i++) {
		str[i] = str[i+1];
	}

	str[len-1] = '\0';
}

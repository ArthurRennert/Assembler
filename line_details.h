#ifndef LINE_DETAILS_H
#define LINE_DETAILS_H
#define MAX_LINE_LENGTH 81
#define MAX_STRING_SIZE 100
#define WORD_IN_MACHINE 12
#define NUMBER_OF_ADDRESSING_TYPES 4
#define MAX_DATA_SIZE 60
#define MAX_OPERATION_LENGTH 4
#define MAX_OPERAND_AND_LABEL_LENGTH 31
#define MAX_FILE_NAME_LENGTH 30


/* An enum with all possible line types. It's used as output form function 'line_parser' from 'parser.c' file */
typedef enum {EMPTY, COMMENT, ENTRY, EXTERN, GUIDANCE_DATA_WITH_LABEL, GUIDANCE_DATA_WITHOUT_LABEL, GUIDANCE_STRING_WITH_LABEL,
GUIDANCE_STRING_WITHOUT_LABEL, INSTRUCTION_WITH_LABEL_2_OPERANDS, INSTRUCTION_WITH_LABEL_1_OPERAND,
INSTRUCTION_WITH_LABEL_NO_OPERANDS, INSTRUCTION_WITHOUT_LABEL_2_OPERANDS,
INSTRUCTION_WITHOUT_LABEL_1_OPERAND, INSTRUCTION_WITHOUT_LABEL_NO_OPERANDS, INVALID_LINE, UNDEFINED_LINE} LINE_TYPE;


/** 
 * An enum with all possible errors in the 'assembler'. The indexes corresponds to the indexes of the 
 * array in 'errors.c' file. So when an error is added, the error description is taken from the array
 * index in 'errors.c' file according to the index of this enum.
 */
typedef enum {LONG_LINE, LABEL_BAD_SYNTAX, LABEL_LENGTH_EXCEED, LABEL_LENGTH_EXCEED_AND_BAD_SYNTAX, LABEL_NAME_LIKE_OPERATION_NAME, LABEL_NAME_LIKE_REGISTER_NAME, LABEL_NAME_LIKE_EXISTING_LABEL, NO_VALUES_AFTER_OPERATION, TOO_MANY_VALUES_AFTER_OPERATION, TOO_SHORT_STRING,
STRING_STARTS_WITHOUT_QUATATION_MARK, STRING_ENDS_WITHOUT_QUATATION_MARK,
STRING_STARTS_AND_ENDS_WITHOUT_QUATATION_MARK, TOO_LONG_STRING, DATA_DOESNT_START_WITH_A_NUMBER, WRONG_CHAR_IN_DATA, MISSING_COMMA_IN_DATA,
TOO_MANY_COMMAS_IN_DATA, MISSING_OPERANDS, TOO_MANY_OPERANDS, INVALID_IMMEDIATE_OPERAND, MISSING_NUMBER_AFTER_NUMBER_SIGN, BAD_SYNTAX_OF_SRC_OPERAND,
BAD_SYNTAX_OF_DEST_OPERAND, BAD_SYNTAX_OF_SRC_AND_DEST_OPERANDS, MISSING_COMMA_BETWEEN_OPERANDS, A_REDUNDANT_COMMA_BETWEEN_OPERANDS,
INVALID_CHARACTER_BETWEEN_OPERANDS, REDUNDANT_INFORMATION_IN_OPERANDS_AREA, BAD_OPERATION_SYNTAX, MISSING_INFORMATION_AFTER_DATA,
MISSING_INFORMATION_AFTER_STRING, UNKNOWN_LINE, MISSING_INFORMATION_AFTER_LABEL, UNDEFINED_LABEL, LABEL_NAME_LIKE_KEYWORD_NAME, 
UNDEFINED_LABEL_IN_DESTINATION_OPERAND, UNDEFINED_LABEL_IN_SOURCE_OPERAND, ENTLY_LABEL_EXISTS_TRY_TO_ADD_EXTERN_LABEL, EXTERN_LABEL_EXISTS_TRY_TO_ADD_ENTRY_LABEL, LABEL_EXISTS_IN_CODE_TRY_TO_ADD_DATA_LABEL,
LABEL_EXISTS_IN_DATA_TRY_TO_ADD_CODE_LABEL, DUPLICATE_CODE_LABEL_ATTEMPT, DUPLICATE_DATA_LABEL_ATTEMPT, UNDEFINED_ENTRY_LABEL, FILE_NAME_LONGER_THAN_ALLOWED,
DATA_DOESNT_END_WITH_A_NUMBER, NO_ERRORS
}
ERR_VALUE;


/**
 * An enum with all possible addressing types in the 'assembler', used for convenience
 * for parsing in 'parser.c' file.
 */
typedef enum {no_type = -1, immediate, direct, relative, regis_direct} addressing_type;


/** 
 * An enum with all possible operations in the 'assembler', used for convenience
 * for parsing in 'parser.c' and as array indexes in 'is_operation' function in 'parser.c' file.
 */
typedef enum {mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rts, stop, no_op} ops;


/** 
 * An enum with all possible attributes in the 'assembler'. Used to pass to 'symbol_table.c' file
 * 'insert' function so it can store the attribute of the label.
 */
typedef enum {no_attr = -1, external, entry, code, data, code_entry, data_entry} attribute;


/** 
 * An enum with all possible registers in the 'assembler'.
 */
typedef enum {r0, r1, r2, r3, r4, r5, r6, r7} registers;


/**
 * A struct used to store all relevant information during parsing, 'parser.c' functions store here
 * all relevant information and 'handler.c' retrieves this relevant information according to it's needs.
 */
typedef struct line_details{
	char label[MAX_OPERAND_AND_LABEL_LENGTH + 1];	/* store here if label was in line, +1 for string terminator char */
	char op[MAX_OPERATION_LENGTH + 1];	/* store here if operation was in line */
	ops en_op; /* enum value of the op, the enum is defined in this file */
	char source[MAX_OPERAND_AND_LABEL_LENGTH + 1];	/* store here if source operand was in line */
	char destination[MAX_OPERAND_AND_LABEL_LENGTH + 1];	/* store here if destination operand was in line */
	addressing_type src_adr;	/* the addressing type of the source operand, addressing type enum is defind in this file */
	addressing_type dest_adr;	/* the addressing type of the destination operand, addressing type enum is defind in this file */
	char string[MAX_STRING_SIZE];	/* used to store the string comes after '.string' here */
	int data[MAX_DATA_SIZE];	/* used to store the data comes after '.data' here */
	int num_in_data_array;	/* used to store the quantity of the data that came after '.data' here */
	char num_to_mem[WORD_IN_MACHINE + 1];	/* store here the number needed for 'memory_image.c' file to retrieve */
	char lbl_to_mem[MAX_OPERAND_AND_LABEL_LENGTH + 1];	/* store here the label needed for 'memory_image.c' file to retrieve */
	ERR_VALUE err;	/* store here the err enum value here for 'handler.c' to trigger an error creation and addition */
} line_details;


#endif

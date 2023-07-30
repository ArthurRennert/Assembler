#ifndef MAIN_FUNCTIONS_H
#define MAIN_FUNCTIONS_H
#include "line_details.h" /* for attribute and ERR_VALUE enums */


/**
 * symb_controller function triggers 'symbol_table.c' file to create and insert an error to the symbol table list.
 * The function must be called ONLY with a valid label.
 * In case the function detected a non valid addressing type, the corresponding err enum value is set to err param.
 * 
 * @param lbl - the label needed to insert to the symbol list.
 * @param IC - the line code number where the label is defined.
 * @param att - the attribute of the label. (attribute enum is defined in 'line_details.h' file).
 * @param err - a pointer to err. (ERR_VALUE enum) so the function can store there the err enum value if it detects an error.
 */
void symb_controller(char* lbl, int IC, attribute att, ERR_VALUE* err);


/**
 * symb_check function searches if a label exists in the symbol table (list) and if the label exists, the function returns the address of the label.
 *
 * @param lbl - the label to search in the symbol table.
 * @return - the address of the label. If label was not found, -1 is returned.
 */
int symb_check(char* lbl);


/**
 * is_extern_lbl function checks whether the label is external. If the label is external, 1 is returned, otherwise 0 is returned.
 *
 * @param lbl - the label to check if it's an external label.
 * @return - 1 is returned if the label is external, otherwise 0 is returned.
 */
int is_extern_lbl(char* lbl);


/**
 * update_symb_table function updates all '.data' and '.string' labels with the offset of IC (Instruction counter).
 * This function must be used ONLY if no errors were found during the execution of the 'assembler'.
 *
 * @param IC - the total lines of instructions.
 */
void update_symb_table(int IC);


/**
 * error_controller function triggers 'error.c' file to create and insert an error to the errors list.
 * This function must be used ONLY if an error has occurred. Once this function is being called, output files will not be created.
 *
 * @param ln - the line in the file where the error has occurred.
 * @param err - the enum value of the error. (enum ERR_VALUE is defined in 'line_details.h' file).
 * @param file_name - the name of the file in which the error has occured.
 */
void error_controller(int ln, ERR_VALUE err, const char* file_name);


/**
 * create_ent_file function creates an output entry file.
 * This function must be used ONLY if no errors were found during the execution of the 'assembler'.
 *
 * @param fn - the name of the file of which the output file is being created.
 */
void create_ent_file(const char* fn);


/**
 * were_errors function check whether errors were found during the 'assembler' execution.
 * This function must be used BEFORE creating output ob, ent, ext files to verify that it's safe to create those files.
 *
 * @return - 0 if no errors were found during the 'assembler' execution, otherwise return 1.
 */
int were_errors();



/**
 * check_entry_labels function checks whether all entry labels defined correctly.
 */
void check_entry_labels();


/**
 * get_file_name function returns the name of the file currently processed.
 *
 * @return - the name of the file currently processed.
 */
char* get_file_name();


#endif /* MAIN_FUNCTIONS_H */

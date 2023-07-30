#ifndef HANDLER_FUNCTIONS_FOR_MEMORY_H
#define HANDLER_FUNCTIONS_FOR_MEMORY_H
#include "line_details.h"  /* for ops enum */


/**
 * get_op function returns the op enum value of the line currently processed in handler execution.
 * ops is an enum defined in 'line_details.h' file.
 *
 * @return - the enum value of the op currently processed in handler execution, if no op is processed, no_op (defined in the enum) is returned.
 */
ops get_op();


/**
 * get_dest_type function returns the addressing type of the destination operand currently processed in handler execution.
 * addressing_type is an enum defined in 'line_details.h' file.
 *
 * @return - the type of the destination operand currently processed in handler execution.
 */
addressing_type get_dest_type();


/**
 * get_src_type function returns the addressing type of the source operand currently processed in handler execution.
 * addressing_type is an enum defined in 'line_details.h' file.
 *
 * @return - the type of the source operand currently processed in handler execution.
 */
addressing_type get_src_type();


/**
 * get_number function returns the number currently proccesed in handler execution.
 * The number is either of a register direct operand or of an immediate operand.
 *
 * Register's r0 number is 1 (in binary - 000000000001)
 * Register's r1 number is 2 (in binary - 000000000010)
 * Register's r2 number is 4 (in binary - 000000000100)
 * same rule (each register's binary representation shifts left logical once from the previous register's binary representation) applies to 
 * registers r3, r4, r5, r6, r7.
 *
 * If the number returned is of an immediate operand, the value returned is a decimal value of the immediate operand.
 *
 * @return - the value of the register or the value of the number currently processed in handler execution.
 */
char* get_number();


/**
 * get_label function returns the label (label as source or destination operands) currently proccesed in handler execution.
 *
 * @return - the label currently processed in handler execution.
 */
char* get_label();


/**
 * get_curr_line function returns the line number currently proccesed in handler execution.
 *
 * @return - the line number currently processed in handler execution.
 */
int get_curr_line();


#endif

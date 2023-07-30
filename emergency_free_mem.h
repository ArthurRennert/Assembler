#ifndef EMERGENCY_FREE_MEM_H
#define EMERGENCY_FREE_MEM_H
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/**
 * emergency_free_mem function frees all memory allocated during the execution. This function is called ONLY if any memory allocation fails.
 * It frees all the memory allocated until first memory allocation failure.
 * This function is not included in any .header file ('only in emergency_free.mem_h') to prevent using it accidentally.
 */
void emergency_free_mem();


/**
 * handler_free_mem function gets called by emergency_free_mem function to free all allocation made in 'handler.c' file.
 * This function is not included in any .header file ('only in emergency_free.mem_h') to prevent using it accidentally.
 */
void handler_free_mem();

#endif

#ifndef STACK_H
#define STACK_H

/**
 * @file stack.h
 * @author GraY
 * @brief Functions and macros for working with the stack.
 */


#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include "types.h"

extern FILE *LOG_FILE; ///< Log file from log.cpp.

/**
 * @brief Macro for convinient and detailed stack dump.
 */
#define STACK_DUMP(stk_descriptor) stack_dump(stk_descriptor, #stk_descriptor, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#ifdef PROTECT
#include "../include/hash_functions.h"

/**
 * @brief Macro for @b stack verification.
 * Return err-code and prints @b format_str with @b __VA_ARGS__ parametres to @b LOG_FILE on stack incorrection.
 */
#define STACK_VERIFICATION(stack_descriptor, err_code, format_str, ...) stack_validation(stack_descriptor); \
                                                                        if(stack_info(stack_descriptor).err.invalid) \
                                                                        { \
                                                                            fprintf(LOG_FILE, format_str, __VA_ARGS__); \
                                                                            \
                                                                            return err_code; \
                                                                        }

/**
 * @brief Macro for stack @b data verification.
 * Return @b EINVAL from errno.h and prints err message to @b LOG_FILE if data is corrupted.
 */
#define STACK_DATA_VERIFICATION(stack_descriptor)   stack_data_validation(stack_descriptor); \
                                                    if(stack_info(stack_descriptor).err.invalid) \
                                                    { \
                                                          fprintf(LOG_FILE, "%s: In %s:%d: error: Corrupted stack data.\n", \
                                                                 __FILE__, __PRETTY_FUNCTION__, __LINE__); \
                                                          \
                                                          return EINVAL; \
                                                    }

/**
 * @brief Macro for @b stack_descriptor verification.
 * Return @b EINVAL from errno.h and prints err message to @b LOG_FILE if @b stack_descriptor is invalid.
 */
#define STACK_DESCRIPTOR_VERIFICATION(stack_descriptor) if(stack_descriptor_validation(stack_descriptor)) \
                                                        { \
                                                            fprintf(LOG_FILE, "%s: In %s: error: Invalid stack descriptor.\n", __FILE__, __PRETTY_FUNCTION__); \
                                                            \
                                                            return EINVAL; \
                                                        }

/**
 * @brief Macro for @b Stack, stack @b data and @b stack_descriptro verification.
 */
#define VERIFICATION(stack_descriptor, ...) STACK_DESCRIPTOR_VERIFICATION(stack_descriptor); \
                                            STACK_VERIFICATION(stack_descriptor, __VA_ARGS__); \
                                            STACK_DATA_VERIFICATION(stack_descriptor);

/**
 * @brief Macro for @b stack and it`s @b data hashing.
 */
#define HASH_STACK(stk_adr) stk_adr->data_hash  = poly_hash_data (stk_adr); \
                            stk_adr->stack_hash = poly_hash_stack(stk_adr)

/**
 * @brief Macro for stack @b data expansion in @b times times.
 *
 */
#define REALLOC_DATA_UP(stack_ptr, times) (elem_t *)((canary_t *)realloc(((canary_t *)stack_ptr->data) - 1, \
                                           sizeof(elem_t) * (stack_ptr->capacity *= times) + 2 * sizeof(canary_t)) + 1)

/**
 * @brief Macro for stack @b data shrinking in @b times times.
*/
#define REALLOC_DATA_DOWN(stack_ptr, times) (elem_t *)((canary_t *)realloc(((canary_t *)stack_ptr->data) - 1, \
                                             sizeof(elem_t) * (stack_ptr->capacity /= times) + 2 * sizeof(canary_t)) + 1)

#else

#define STACK_VERIFICATION(...)

#define STACK_DATA_VERIFICATION(...)

#define STACK_DESCRIPTOR_VERIFICATION(stack_descriptor)

#define VERIFICATION(...)

#define HASH_STACK(...)

#define REALLOC_DATA_UP(stack_ptr, times) (elem_t *)realloc(stack_ptr->data, sizeof(elem_t) * (stack_ptr->capacity *= times))

#define REALLOC_DATA_DOWN(stack_ptr, times) (elem_t *)realloc(stack_ptr->data, sizeof(elem_t) * (stack_ptr->capacity /= times))

#endif
/**
 * @brief @b Stack constructor.
 * Generates @b Stack with given capacity and writes it`s descriptor to a @b stack_descriptor if succeded, otherwise @b 0;
 * @param stack_descriptor Pointer to stack descriptor.
 * @param capacity Capacity of generated stack.
 * @return int Error code.
 */
int stack_ctor(stk_d *stack_descriptor, const size_t capacity);

/**
 * @brief @b Stack destructor.
 * @param stack_descriptor Stack descriptor.
 * @return int Error code.
 */
int stack_dtor(const stk_d stack_descriptor);

/**
 * @brief Function for pushing elements ito the stack.
 * @param stack_descriptor Stack descriptor.
 * @param val Element value to push.
 * @return int Error code.
 */
int push_stack(const stk_d stack_descriptor, const elem_t val);

/**
 * @brief Function for removing elements from the stack.
 * @param stack_descriptor Stack descriptor.
 * @param ret_val If not @b NULL, writes removed value to @b ret_val.
 * @return int Error code.
 */
int pop_stack(const stk_d stack_descriptor, elem_t *ret_val = NULL);

/**
 * @brief Function for @b Stack data expansion, if needed more space.
 * @param stack_descriptor Stack descriptor.
 * @return int Error code.
 */
int optimal_expansion(const stk_d stack_descriptor);

/**
 * @brief Function for @b Stack data expansion, if there is too much free space.
 * @param stack_descriptor Stack descriptor.
 * @return int Error code.
 */
int optimal_shrink(const stk_d stack_descriptor);


int clear_stack(const stk_d stack_descriptor);

/**
 * @brief
 *
 * @param stack_descriptor Stack descriptor.
 * @param stack_name Name of the @b Stack given.
 * @param file_name Name of the file from which function was called.
 * @param func_declaration Function declaration.
 * @param line Number of line where function was called.
 * @return int Error code.
 */
int stack_dump(const stk_d stack_descriptor, const char *stack_name, const char *file_name, const char * func_declaration, const int line);


struct Stack stack_info(const stk_d stack_descriptor);

#ifdef PROTECT
int stack_descriptor_validation(const stk_d stack_descriptor);

void stack_validation(const stk_d stack_descriptor);

void stack_data_validation(const stk_d stack_descriptor);
#endif

#endif //STACK_H
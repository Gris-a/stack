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

#ifndef NDEBUG
#include "../include/hash_functions.h"

/**
 * @brief Macro for @b stack verification.
 * Return err-code and prints @b format_str with @b __VA_ARGS__ parametres to @b LOG_FILE on stack incorrection.
 */
#define STACK_VERIFICATION(Stacks, stack_descriptor, err_code, format_str, ...) stack_validation(Stacks + stack_descriptor); \
                                                                                if((Stacks + stack_descriptor)->err.invalid) \
                                                                                { \
                                                                                   fprintf(LOG_FILE, format_str, __VA_ARGS__); \
                                                                                   STACK_DUMP(stack_descriptor); \
                                                                                   return err_code; \
                                                                                }

/**
 * @brief Macro for stack @b data verification.
 * Return @b EINVAL from errno.h and prints err message to @b LOG_FILE if data is corrupted.
 */
#define STACK_DATA_VERIFICATION(Stacks, stack_descriptor) stack_data_validation(Stacks + stack_descriptor); \
                                                          if((Stacks + stack_descriptor)->err.invalid) \
                                                          { \
                                                               fprintf(LOG_FILE, "%s: In %s:%d: error: Corrupted stack data.\n", \
                                                                       __FILE__, __PRETTY_FUNCTION__, __LINE__); \
                                                               STACK_DUMP(stack_descriptor); \
                                                               return EINVAL; \
                                                          }
/**
 * @brief Macro for @b stack_descriptor verification.
 * Return @b EINVAL from errno.h and prints err message to @b LOG_FILE if @b stack_descriptor is invalid.
 */
#define STACK_DESCRIPTOR_VERIFICATION(stack_descriptor) if(stack_descriptor >= N_buffered_stacks) \
                                                        { \
                                                            fprintf(LOG_FILE, "%s: In %s: error: Invalid stack descriptor.\n", __FILE__, __PRETTY_FUNCTION__); \
                                                            \
                                                            return EINVAL; \
                                                        }
/**
 * @brief Macro for @b Stack, stack @b data and @b stack_descriptro verification.
 */
#define VERIFICATION(Stacks, stack_descriptor, ...) STACK_DESCRIPTOR_VERIFICATION(stack_descriptor); \
                                                    STACK_VERIFICATION(Stacks, stack_descriptor, __VA_ARGS__); \
                                                    STACK_DATA_VERIFICATION(Stacks, stack_descriptor); \

/**
 * @brief Macro for @b stack and it`s @b data hashing.
 */
#define HASH_STACK(stk_adr) stk_adr->data_hash  = poly_hash_data (stk_adr); \
                            stk_adr->stack_hash = poly_hash_stack(stk_adr)
#else

#define STACK_VERIFICATION(...)

#define STACK_DATA_VERIFICATION(...)

#define STACK_DESCRIPTOR_VERIFICATION(stack_descriptor)

#define VERIFICATION(...)

#define HASH_STACK(...)

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

#ifndef NDEBUG
/**
 * @brief Fuction for @b stack correction check.
 * Fills @b Err fields in @b stack if somethinf wrong.
 * @param stack Pointer to the @b Stack.
 */
void stack_validation(struct Stack *stack);

/**
 * @brief Fuction for stack @b data correction check.
 * Fills @b Err fields in @b stack if somethinf wrong.
 * @param stack Pointer to the @b Stack.
 */
void stack_data_validation(struct Stack *stack);
#endif

#endif //STACK_H
#ifndef STACK_H
#define STACK_H

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include "types.h"

extern FILE *LOG_FILE;

#define STACK_DUMP(stk_descriptor) stack_dump(stk_descriptor, #stk_descriptor, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#ifndef NDEBUG
#include "../include/hash_functions.h"

#define STACK_VERIFICATION(stack_ptr, err_code, format_str, ...) stack_validation(stack_ptr); \
                                                                 if(stack_ptr->err.invalid) \
                                                                 { \
                                                                  fprintf(LOG_FILE, format_str, __VA_ARGS__); \
                                                                  return err_code; \
                                                                 }

#define STACK_DATA_VERIFICATION(stack_ptr) stack_data_validation(stack_ptr); \
                                           if(stack_ptr->err.invalid) \
                                           { \
                                               fprintf(LOG_FILE, "%s: In %s:%d: error: Corrupted stack data.\n", \
                                                       __FILE__, __PRETTY_FUNCTION__, __LINE__); \
                                               return EINVAL; \
                                           }

#define STACK_DESCRIPTOR_VERIFICATION(stack_descriptor) if(stack_descriptor >= N_buffered_stacks) \
                                                        { \
                                                            fprintf(LOG_FILE, "%s: In %s: error: Invalid stack descriptor.\n", __FILE__, __PRETTY_FUNCTION__); \
                                                            \
                                                            return EINVAL; \
                                                        }

#define HASH_STACK(stk_adr) stk_adr->data_hash  = poly_hash_data (stk_adr); \
                            stk_adr->stack_hash = poly_hash_stack(stk_adr)

#else

#define STACK_VERIFICATION(...)

#define STACK_DATA_VERIFICATION(...)

#define STACK_DESCRIPTOR_VERIFICATION(stack_descriptor)

#define HASH_STACK(...)

#endif

int stack_ctor(stk_d *stack_descriptor, const size_t capacity);

int stack_dtor(const stk_d stack_descriptor);

int push_stack(const stk_d stack_descriptor, const elem_t val);

int pop_stack(const stk_d stack_descriptor, elem_t *ret_val = NULL);

int optimal_expansion(const stk_d stack_descriptor);

int optimal_shrink(const stk_d stack_descriptor);

int stack_dump(const stk_d stack_descriptor, const char *stack_name, const char *file_name, const char * func_declaration, const int line);

#ifndef NDEBUG
void stack_validation(struct Stack *stack);

void stack_data_validation(struct Stack *stack);
#endif

void free_stack_mem(void);

#endif //STACK_H
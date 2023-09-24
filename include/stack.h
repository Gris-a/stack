#ifndef STACK_H
#define STACK_H

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

extern FILE *LOG_FILE;

#ifndef NDEBUG
#define STACK_DUMP(stk) stack_dump(&stk, #stk, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#define STACK_VERIFICATION(stack_ptr, err_code, format_str, ...) stack_validation(stack); \
                                                                 if(stack->err.invalid) \
                                                                 { \
                                                                  fprintf(LOG_FILE, format_str, __VA_ARGS__); \
                                                                  return err_code; \
                                                                 }

#define STACK_DATA_VERIFICATION(stack_ptr) stack_data_validation(stack); \
                                                if(stack->err.invalid) \
                                                { \
                                                    fprintf(LOG_FILE, "%s: In %s:%d: error: Corrupted stack data.\n", \
                                                            __FILE__, __PRETTY_FUNCTION__, __LINE__); \
                                                    return EINVAL; \
                                                }

#else

#define STACK_VERIFICATION(stack_ptr) ;

#define STACK_DATA_VERIFICATION(stack_ptr) ;

#endif

typedef int elem_t;
typedef unsigned long long canary_t;

const size_t BASE_CAPACITY = 10;
const canary_t CANARY_VAL  = 0xB1BAB0BA;

struct Err
{
    unsigned int invalid  :1;
    unsigned int no_data  :1;
    unsigned int sizeless :1;
    unsigned int overflow :1;
    unsigned int underflow:1;
};

struct Stack
{
    canary_t canary_left;

    size_t size;
    size_t capacity;

    elem_t *data;

    struct Err err;

    canary_t canary_right;
};

int stack_ctor(struct Stack *stack, const size_t capacity = BASE_CAPACITY);

void stack_dtor(struct Stack *stack);

int push_stack(struct Stack *stack, const elem_t val);

int pop_stack(struct Stack *stack, elem_t *ret_val = NULL);

int optimal_expansion(struct Stack *stack);

int optimal_shrink(struct Stack *stack);

void stack_validation(struct Stack *stack);

void stack_data_validation(struct Stack *stack);

void stack_dump(const struct Stack *stack, const char *stack_name, const char *file_name, const char * func_declaration, const int line);

#endif //STACK_H
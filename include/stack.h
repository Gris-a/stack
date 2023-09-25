#ifndef STACK_H
#define STACK_H
//TODO hash of stack(no data). Hash is not biective and canary, hash create and use if defined. Chack if can remove ndebug and place here
#include <errno.h>
#include <stddef.h>
#include <stdio.h>

extern FILE *LOG_FILE;

#define STACK_DUMP(stk) stack_dump(&stk, #stk, __FILE__, __PRETTY_FUNCTION__, __LINE__)


typedef long long elem_t;
typedef unsigned long long canary_t;

const size_t Base_capacity = 10;
const canary_t Canary_val  = 0xB1BAB0BA;

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
    #ifndef NDEBUG
    canary_t canary_left;
    #endif

    size_t size;
    size_t capacity;

    elem_t *data;

    #ifndef NDEBUG
    size_t  data_hash;

    struct Err err;

    canary_t canary_right;
    #endif
};

#ifndef NDEBUG
#include "../include/hash_functions.h"

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
#define HASH_DATA(stk_adr) poly_hash(stk_adr, &stk_adr->data_hash)

#else

#define STACK_VERIFICATION(...)

#define STACK_DATA_VERIFICATION(...)

#define HASH_DATA(...)

#endif

int stack_ctor(struct Stack *stack, const size_t capacity = Base_capacity);

void stack_dtor(struct Stack *stack);

int push_stack(struct Stack *stack, const elem_t val);

int pop_stack(struct Stack *stack, elem_t *ret_val = NULL);

int optimal_expansion(struct Stack *stack);

int optimal_shrink(struct Stack *stack);

void stack_dump(const struct Stack *stack, const char *stack_name, const char *file_name, const char * func_declaration, const int line);

#ifndef NDEBUG
void stack_validation(struct Stack *stack);

void stack_data_validation(struct Stack *stack);
#endif

#endif //STACK_H
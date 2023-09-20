#ifndef STACK_H
#define STACK_H

#include <stddef.h>

typedef int elem_t;

const size_t BASE_CAPACITY = 10;

struct Err
{
    char destroyed:1;

    char invalid  :1;
    char no_data  :1;
    char sizeless :1;
    char overflow :1;
    char underflow:1;
};

struct Stack
{
    size_t size;
    size_t capacity;

    elem_t *data;

    struct Err err;
};

int stack_ctor(struct Stack *stack, const size_t capacity = BASE_CAPACITY);

void stack_dtor(struct Stack *stack);

int push_stack(struct Stack *stack, const elem_t val);

int pop_stack(struct Stack *stack, elem_t *ret_val = NULL);

int optimal_expansion(struct Stack *stack);

int optimal_shrink(struct Stack *stack);

void stack_validation(struct Stack *stack);

void stack_dump(const struct Stack *stack);

#endif //STACK_H
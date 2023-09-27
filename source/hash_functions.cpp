#include <assert.h>
#include <stddef.h>

#include "../include/hash_functions.h"
#include "../include/stack.h"

static const unsigned long long P = 257;

size_t poly_hash_data(Stack *stack)
{
    assert(stack != NULL);


    if(stack->data == NULL)
    {
        return 0;
    }

    size_t hash      = 0;
    size_t powered_P = 1;

    for(size_t i = 0; i < stack->capacity * sizeof(elem_t); i++)
    {
        hash      += (size_t)((char *)stack->data)[i] * powered_P;
        powered_P *= P;
    }


    return hash;
}

size_t poly_hash_stack(Stack *stack)
{
    assert(stack != NULL);


    size_t hash_prev  = stack->stack_hash;
    stack->stack_hash = 0;

    size_t hash_val  = 0;
    size_t powered_P = 1;

    for(size_t i = 0; i < sizeof(Stack); i++)
    {
        hash_val  += (size_t)((char *)stack)[i] * powered_P;
        powered_P *= P;
    }

    stack->stack_hash = hash_prev;


    return hash_val;
}
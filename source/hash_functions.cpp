#include <assert.h>
#include <stddef.h>

#include "../include/hash_functions.h"
#include "../include/stack.h"

void poly_hash_data(Stack *stack, size_t *hash)
{
    assert(stack != NULL);
    assert(hash  != NULL);

    *hash = 0;
    if(stack->data == NULL)
    {
        return;
    }

    size_t powered_P = 1;

    for(size_t i = 0; i < stack->capacity; i++)
    {
        *hash     += (size_t)stack->data[i] * powered_P;
        powered_P *= P;
    }
}

void poly_hash_stack(Stack *stack, size_t *hash)
{
    assert(stack != NULL);
    assert(hash  != NULL);

    stack->stack_hash = 0;

    char *byte_stk = (char *)stack;

    *hash            = 0;
    size_t powered_P = 1;

    for(size_t i = 0; i < sizeof(Stack); i++)
    {
        *hash     += (size_t)(*byte_stk++) * powered_P;
        powered_P *= P;
    }

    stack->stack_hash = *hash;
}
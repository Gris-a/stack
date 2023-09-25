#include <assert.h>
#include <stddef.h>

#include "../include/hash_functions.h"
#include "../include/stack.h"

void poly_hash(const Stack *stack, size_t *hash)
{
    assert(stack != NULL);
    assert(hash  != NULL);

    *hash            = 0;
    size_t powered_P = 1;

    for(size_t i = 0; i < stack->size; i++)
    {
        *hash += (size_t)stack->data[i] * powered_P;
        powered_P *= P;
    }
}
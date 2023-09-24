#include <stdio.h>
#include <stdlib.h>

#include "../include/stack.h"

int main(void)
{
    struct Stack stack = {};
    stack_ctor(&stack, 2);
    for(int i = 0; i < 100; i++)
    {
        push_stack(&stack, i);
    }
    for(int i = 0; i < 100; i++)
    {
        pop_stack(&stack);

    }
    STACK_DUMP(stack);
    stack_dtor(&stack);
    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>

#include "../include/stack.h"
#include "../include/log.h"

int main(void)
{
    struct Stack stack = {};
    stack_ctor(&stack, 1);
    for(int i = 0; i < 10; i++)
    {
        push_stack(&stack, i);
    }
    STACK_DUMP(stack);
    stack_dtor(&stack);
    return EXIT_SUCCESS;
}
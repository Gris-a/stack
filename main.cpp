#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

int main(void)
{
    struct Stack stack = {};
    stack_ctor(&stack, 1);
    push_stack(&stack, 10);

    stack_dtor(&stack);
    return EXIT_SUCCESS;
}
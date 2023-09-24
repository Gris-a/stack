#include <stdio.h>
#include <stdlib.h>

#include "../include/stack.h"
#include "../include/log.h"

int main(void)
{
    struct Stack stack = {};
    stack_ctor(&stack, 2);

    STACK_DUMP(stack);
    stack_dtor(&stack);
    return EXIT_SUCCESS;
}
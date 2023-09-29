#include "../include/stack.h"
#include <stdio.h>

int main(void)
{
    stk_d stack1 = 0;
    stk_d stack2 = 0;
    stack_ctor(&stack1, 10);
    stack_ctor(&stack2, 10);
    push_stack(stack1, 1);
    push_stack(stack2, 1);
    STACK_DUMP(stack1);
    STACK_DUMP(stack2);
}
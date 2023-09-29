#include "../include/stack.h"
#include <stdio.h>

extern struct Stack Stacks[];

int main(void)
{
    stk_d stack1 = 0;
    stk_d stack2 = 0;

    stack_ctor(&stack1, 10);
    stack_ctor(&stack2, 10);

    Stack * a = Stacks +1;

    *(int *)a = 5;

    push_stack(stack1, 1);

    stack_dtor(stack1);
    stack_dtor(stack2);
}
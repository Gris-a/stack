#include "../include/stack.h"
#include <stdio.h>

extern struct Stack Stacks[];

int main(void)
{
    stk_d stack1 = 0;

    stack_ctor(&stack1, 10);

    for(int i = 0; i < 1000; i++)
        push_stack(stack1, 1);
    for(int i = 0; i < 1000; i++)
        pop_stack(stack1);

    stack_dtor(stack1);
}
#include "../include/stack.h"
#include "../include/log.h"
#include "../include/hash_functions.h"

int main(void)
{

    stk_d stack = 0;
    stack_ctor(&stack, 10);
    for(int i = 0; i < 1000; i++)
    {
        push_stack(stack, 10);
    }
    for(int i = 0; i < 1000; i++)
    {
        pop_stack(stack);
    }

    STACK_DUMP(stack);
    stack_dtor(stack);
}
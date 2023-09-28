#include "../include/stack.h"
#include "../include/log.h"
#include "../include/hash_functions.h"

int main(void)
{
    stk_d s1 = 0;
    stk_d s2 = 0;
    stack_ctor(&s1, 10);
    stack_ctor(&s2, 10);
    push_stack(s1, 1);
    push_stack(s2, 2);

    STACK_DUMP(s1);
    STACK_DUMP(s2);
    stack_dtor(s1);
    stack_dtor(s2);

}
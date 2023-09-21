#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/stack.h"

int stack_ctor(struct Stack *stack, const size_t capacity)
{
    assert(stack);
    if(capacity == 0)
    {
        printf("%s: In %s: error: Capasity should be greater than zero.\n", __FILE__, __PRETTY_FUNCTION__);
        return EINVAL;
    }

    stack->err      = {};
    stack->size     = 0;
    stack->capacity = capacity;

    stack->data = (elem_t *)calloc(capacity, sizeof(elem_t));

    STACK_VERIFICATION(stack, ENOMEM, "%s: In %s:%d: error: Unable to allocate memory.\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

    return EXIT_SUCCESS;
}

void stack_dtor(struct Stack *stack)
{
    assert(stack);

    stack->size     = 0;
    stack->capacity = 0;
    free(stack->data);

    stack->err          = {};
    stack->err.invalid  = true;
    stack->err.sizeless = true;
    stack->err.no_data  = true;
}

int push_stack(struct Stack *stack, const elem_t val)
{
    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    int err_code = 0;
    if((err_code = optimal_expansion(stack)))
    {
        printf("%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

        return err_code;
    }

    stack->data[stack->size++] = val;

    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    return EXIT_SUCCESS;
}

int pop_stack(struct Stack *stack, elem_t *ret_val)
{
    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    if(stack->size == 0)
    {
        printf("Error: stack underflow.\n"
               "%s: In function %s\n", __FILE__, __PRETTY_FUNCTION__);

        stack->err.invalid   = true;
        stack->err.underflow = true;

        return EINVAL;
    }

    stack->data[stack->size--] = 0;
    elem_t value = stack->data[stack->size];

    if(ret_val) *ret_val = value;

    int err_code = 0;
    if((err_code = optimal_shrink(stack)))
    {
        printf("%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

        return err_code;
    }

    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    return EXIT_SUCCESS;
}

int optimal_expansion(struct Stack *stack)
{
    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    if(stack->size == stack->capacity)
    {
        stack->data = (elem_t *)realloc(stack->data, sizeof(elem_t) * (stack->capacity *= 2));
    }

    STACK_VERIFICATION(stack, ENOMEM, "Error: unable to reallocate memory.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 4);

    return EXIT_SUCCESS;
}

int optimal_shrink(struct Stack *stack)
{
    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    if(stack->size * 4 == stack->capacity)
    {
        stack->data = (elem_t *)realloc(stack->data, sizeof(elem_t) * (stack->capacity /= 2));
    }

    STACK_VERIFICATION(stack, ENOMEM, "Error: unable to reallocate memory.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 4);

    return EXIT_SUCCESS;
}

void stack_validation(struct Stack *stack)
{
    assert(stack);

    stack->err.no_data  = (stack->data == NULL);

    stack->err.overflow = (!stack->err.sizeless && stack->size > stack->capacity);

    stack->err.invalid  = (stack->err.no_data  || stack->err.sizeless ||
                           stack->err.overflow || stack->err.underflow);
}

void stack_dump(const struct Stack *stack, const char *stack_name, const char *file_name, const char * func_declaration, const int line)
{
    assert(stack);
    assert(stack_name);
    assert(file_name);
    assert(func_declaration);

    printf("Stack[%p] \"%s\" from %s\n"
           "In function %s:%d\n", stack, stack_name, file_name, func_declaration, line);

    printf("{                 \n"
            "\tsize     = %zu;\n"
            "\tcapacity = %zu;\n"
            "\tdata[%p]       \n", stack->size, stack->capacity, stack->data);

    if(!stack->err.no_data && !stack->err.sizeless)
    {
        printf("\t{\n");
        for(size_t i = 0; i < stack->capacity; i++)
        {
            printf("\t\t");
            printf((i < stack->size) ? "*" : " ");
            printf("[%3zu] = %d,\n", i, stack->data[i]);
        }
        printf("\t};\n");
    }
    printf("}\n");
}
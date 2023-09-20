#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

int stack_ctor(struct Stack *stack, const size_t capacity)
{
    assert(stack);

    stack->err      = {};
    stack->size     = 0;
    stack->capacity = capacity;

    stack->data = (elem_t *)calloc(capacity, sizeof(elem_t));
    if(!stack->data)
    {
        stack->err.invalid = true;
        stack->err.no_data = true;

        if(capacity > 0)
        {
            printf("Unable to allocate memory.\n"); //TODO to log-file and more info

            return ENOMEM;
        }
        else
        {
            stack->err.sizeless = true;

            return EINVAL;
        }
    }

    return EXIT_SUCCESS;
}

void stack_dtor(struct Stack *stack)
{
    if(stack)
    {
        stack->size          = 0;
        stack->capacity      = 0;
        stack->err           = {};
        stack->err.destroyed = true;

        free(stack->data);
    }
}

int push_stack(struct Stack *stack, const elem_t val) //TODO val validation check
{
    assert(stack);
    stack_validation(stack);
    if(stack->err.invalid || stack->err.destroyed)
    {
        return EINVAL;
    }

    if(optimal_expansion(stack))
    {
        return (stack->err.invalid) ? EINVAL : ENOMEM;
    }

    stack->data[stack->size++] = val;

    return EXIT_SUCCESS;
}

int pop_stack(struct Stack *stack, elem_t *ret_val)
{
    assert(stack);
    stack_validation(stack);
    if(stack->err.invalid || stack->err.destroyed)
    {
        return EINVAL;
    }

    if(stack->size == 0)
    {
        stack->err.invalid   = true;
        stack->err.underflow = true;

        return EINVAL;
    }

    elem_t value = stack->data[--stack->size];

    if(ret_val) *ret_val = value;

    if(optimal_shrink(stack))
    {
        return (stack->err.invalid) ? EINVAL : ENOMEM;
    }

    return EXIT_SUCCESS;
}

int optimal_expansion(struct Stack *stack)
{
    assert(stack);
    stack_validation(stack);
    if(stack->err.invalid || stack->err.destroyed)
    {
        return EINVAL;
    }

    if(stack->size == stack->capacity)
    {
        stack->data = (elem_t *)realloc(stack->data, sizeof(elem_t) * (stack->capacity *= 2));
        if(!stack->data)
        {
            printf("Unable to reallocate memory.\n"); //TODO to log-file and more info
            stack->err.invalid = true;
            stack->err.no_data = true;

            return ENOMEM;
        }
    }

    return EXIT_SUCCESS;
}

int optimal_shrink(struct Stack *stack)
{
    assert(stack);
    stack_validation(stack);
    if(stack->err.invalid || stack->err.destroyed)
    {
        return EINVAL;
    }

    if(stack->size * 4 == stack->capacity)
    {
        stack->data = (elem_t *)realloc(stack->data, sizeof(elem_t) * (stack->capacity /= 2));

        if(!stack->data)
        {
            printf("Unable to reallocate memory.\n"); //TODO to log-file and more info
            stack->err.invalid = true;
            stack->err.no_data = true;

            return ENOMEM;
        }
    }

    return EXIT_SUCCESS;
}

void stack_validation(struct Stack *stack)
{
    assert(stack);

    if(stack->err.destroyed) return;

    stack->err.no_data  = (stack->data == NULL);

    stack->err.overflow = (!stack->err.sizeless && stack->size > stack->capacity + 1);

    stack->err.invalid  = (stack->err.no_data  || stack->err.sizeless ||
                           stack->err.overflow || stack->err.underflow);
}

void stack_dump(const struct Stack *stack)
{
    assert(stack);

    // printf("Stack[%p] \"%s\" from %s(%d)"); //TODO general info

    if(stack->err.destroyed)
    {
        printf("Stack used after destructor without reinitialization.\n");

        return;
    }
    //TODO others err messages, mayby not here
    printf("{                 \n"
            "\tsize     = %zu;\n"
            "\tcapacity = %zu;\n"
            "\tdata[%p]       \n", stack->size, stack->capacity, stack->data);

    if(stack->data)
    {
        printf("\t{\n");
        for(size_t i = 0; i < stack->capacity; i++)
        {
            printf("\t\t");
            printf((i < stack->size) ? "*" : " ");
            printf("[%2zu] = %zu,\n", i, stack->data[i]); //TODO not for any type and not for any number of elements
        }
        printf("\t};\n"
                "} \n");
    }
}
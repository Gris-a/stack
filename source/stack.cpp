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
        fprintf(LOG_FILE, "%s: In %s: error: Capasity should be greater than zero.\n", __FILE__, __PRETTY_FUNCTION__);

        return EINVAL;
    }

    stack->capacity = capacity;
    stack->size     = 0;

    #ifndef NDEBUG
    stack->canary_left  = Canary_val;
    stack->canary_right = Canary_val;
    stack->data_hash    = 0;
    stack->err          = {};

    stack->data = (elem_t *)((canary_t *)calloc(capacity * sizeof(elem_t) + 2 * sizeof(canary_t), sizeof(char)) + 1);

    STACK_VERIFICATION(stack, ENOMEM, "%s: In %s:%d: error: Unable to allocate memory.\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

    ((canary_t *)stack->data)[-1]                = Canary_val;
    *(canary_t *)(stack->data + stack->capacity) = Canary_val;
    #else
    stack->data = (elem_t *)calloc(capacity * sizeof(elem_t), sizeof(char));
    #endif

    return EXIT_SUCCESS;
}

void stack_dtor(struct Stack *stack)
{
    assert(stack);

    stack->size     = 0;
    stack->capacity = 0;

    #ifndef NDEBUG
    stack->data_hash    = 0;

    stack->canary_left  = 0;
    stack->canary_right = 0;

    stack->err          = {};
    stack->err.invalid  = true;
    stack->err.sizeless = true;
    stack->err.no_data  = true;

    free((canary_t *)stack->data - 1);
    #else
    free(stack->data);
    #endif
}

int push_stack(struct Stack *stack, const elem_t val)
{
    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);
    STACK_DATA_VERIFICATION(stack);

    int err_code = 0;
    if((err_code = optimal_expansion(stack)))
    {
        fprintf(LOG_FILE, "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

        return err_code;
    }

    stack->data[stack->size++] = val;

    HASH_DATA(stack);

    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);
    STACK_DATA_VERIFICATION(stack);

    return EXIT_SUCCESS;
}

int pop_stack(struct Stack *stack, elem_t *ret_val)
{
    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);
    STACK_DATA_VERIFICATION(stack);

    if(stack->size == 0)
    {
        fprintf(LOG_FILE, "Error: stack underflow.\n"
               "%s: In function %s\n", __FILE__, __PRETTY_FUNCTION__);

        #ifndef NDEBUG
        stack->err.invalid   = true;
        stack->err.underflow = true;
        #endif

        return EINVAL;
    }

    stack->data[stack->size--] = 0;
    elem_t value = stack->data[stack->size];

    HASH_DATA(stack);

    if(ret_val) *ret_val = value;

    int err_code = 0;
    if((err_code = optimal_shrink(stack)))
    {
        fprintf(LOG_FILE, "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

        return err_code;
    }

    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);
    STACK_DATA_VERIFICATION(stack);

    return EXIT_SUCCESS;
}

int optimal_expansion(struct Stack *stack)
{
    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);
    STACK_DATA_VERIFICATION(stack);

    if(stack->size == stack->capacity)
    {
        #ifndef NDEBUG
        elem_t *temp_ptr = (elem_t *)((canary_t *)realloc(((canary_t *)stack->data) - 1,
                                                            sizeof(elem_t) * (stack->capacity *= 2) + 2 * sizeof(canary_t)) + 1);
        #else
        elem_t *temp_ptr = (elem_t *)realloc(stack->data, sizeof(elem_t) * (stack->capacity *= 2));
        #endif

        if(!temp_ptr)
        {
            fprintf(LOG_FILE, "Error: unable to reallocate memory.\n"
                              "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 5);
            return ENOMEM;
        }

        stack->data = temp_ptr;

        for(size_t i = stack->size; i < stack->capacity; i++)
        {
            stack->data[i] = 0;
        }

        #ifndef NDEBUG
        *(canary_t *)(stack->data + stack->capacity) = Canary_val;
        #endif
    }

    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);
    STACK_DATA_VERIFICATION(stack);

    return EXIT_SUCCESS;
}

int optimal_shrink(struct Stack *stack)
{
    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);
    STACK_DATA_VERIFICATION(stack);

    if(stack->size * 4 == stack->capacity)
    {
        #ifndef NDEBUG
        elem_t *temp_ptr = (elem_t *)((canary_t *)realloc(((canary_t *)stack->data) - 1,
                                                            sizeof(elem_t) * (stack->capacity /= 2) + 2 * sizeof(canary_t)) + 1);
        #else
        elem_t *temp_ptr = (elem_t *)realloc(stack->data, sizeof(elem_t) * (stack->capacity /= 2));
        #endif

        if(!temp_ptr)
        {
            fprintf(LOG_FILE, "Error: unable to reallocate memory.\n"
                              "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 5);
            return ENOMEM;
        }

        stack->data = temp_ptr;

        #ifndef NDEBUG
        *(canary_t *)(stack->data + stack->capacity) = Canary_val;
        #endif
    }

    STACK_VERIFICATION(stack, ENOMEM, "Error: unable to reallocate memory.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 5);
    STACK_DATA_VERIFICATION(stack);

    return EXIT_SUCCESS;
}

void stack_dump(const struct Stack *stack, const char *stack_name, const char *file_name, const char * func_declaration, const int line)
{
    assert(stack);
    assert(stack_name);
    assert(file_name);
    assert(func_declaration);

    fprintf(LOG_FILE, "Stack[%p] \"%s\" from %s\n"
                      "In function %s:%d\n", stack, stack_name, file_name, func_declaration, line);
    #ifndef NDEBUG
    fprintf(LOG_FILE, "{                     \n"
                      "\tcanary_left = %#llx;\n"
                      "\terr         = %u;   \n", stack->canary_left, *(const unsigned int *)(&stack->err));
    #endif

    fprintf(LOG_FILE, "{                     \n"
                      "\tsize        = %zu;  \n"
                      "\tcapacity    = %zu;  \n"
                      "\tdata[%p]            \n", stack->size, stack->capacity, stack->data);

    #ifndef NDEBUG
    if(!stack->err.no_data && !stack->err.sizeless)
    {
        fprintf(LOG_FILE, "\t{\n");

        fprintf(LOG_FILE, "\t\t CANARY_LEFT  = %#llx;\n", ((canary_t *)stack->data)[-1]);

        for(size_t i = 0; i < stack->capacity; i++)
        {
            fprintf(LOG_FILE, "\t\t");
            fprintf(LOG_FILE, (i < stack->size) ? "*" : " ");
            fprintf(LOG_FILE, "[%3zu] = %lld,\n", i, stack->data[i]);
        }

        fprintf(LOG_FILE, "\t\t CANARY_RIGHT = %#llx;\n", *(canary_t *)(stack->data + stack->capacity));

        fprintf(LOG_FILE, "\t};\n");
    }
    fprintf(LOG_FILE, "\tcanary_right = %#llx;\n", stack->canary_right);
    #else
    fprintf(LOG_FILE, "\t{\n");

    for(size_t i = 0; i < stack->capacity; i++)
    {
        fprintf(LOG_FILE, "\t\t");
        fprintf(LOG_FILE, (i < stack->size) ? "*" : " ");
        fprintf(LOG_FILE, "[%3zu] = %lld,\n", i, stack->data[i]);
    }

    fprintf(LOG_FILE, "\t};\n");
    #endif

    fprintf(LOG_FILE, "}\n");
}

#ifndef NDEBUG
void stack_validation(struct Stack *stack)
{
    assert(stack);

    stack->err = {};

    stack->err.no_data  = (stack->data == NULL);

    stack->err.sizeless = (stack->capacity == 0);

    stack->err.overflow = (!stack->err.sizeless && stack->size > stack->capacity);

    stack->err.invalid  = (stack->err.no_data  || stack->err.sizeless || stack->err.overflow ||
                           stack->canary_left != Canary_val || stack->canary_right != Canary_val);
}

void stack_data_validation(struct Stack *stack)
{
    assert(stack);

    size_t hash_val = 0;
    poly_hash(stack, &hash_val);

    if(((canary_t *)stack->data)[-1] != Canary_val || *(canary_t *)(stack->data + stack->capacity) != Canary_val ||
         hash_val != stack->data_hash)
    {
        stack->err.invalid = true;
    }
}
#endif
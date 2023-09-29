/**
 * @file stack.cpp
 * @author GraY
 * @brief Stack functions definitions.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/stack.h"

static size_t N_buffered_stacks = 1;
static const int Max_stacks     = 100;
struct Stack Stacks[Max_stacks] = {}; //static

int stack_ctor(stk_d *stack_descriptor, const size_t capacity)
{
    static size_t Stack_D = 1;

    assert(Stacks);
    assert(stack_descriptor);

    *stack_descriptor = 0;

    if(capacity == 0)
    {
        fprintf(LOG_FILE, "%s: In %s: error: Capasity should be greater than zero.\n", __FILE__, __PRETTY_FUNCTION__);

        return EINVAL;
    }

    if(Stack_D >= Max_stacks)
    {
        fprintf(LOG_FILE, "%s: In %s: error: Max stacks limit reached.\n", __FILE__, __PRETTY_FUNCTION__);

        return EACCES;
    }

    N_buffered_stacks++;
    struct Stack *stack = Stacks + Stack_D;

    stack->capacity = capacity;

#ifdef PROTECT
    stack->canary_left  = Canary_val;
    stack->canary_right = Canary_val;

    stack->data = (elem_t *)((canary_t *)calloc(capacity * sizeof(elem_t) + 2 * sizeof(canary_t), sizeof(char)) + 1);

    HASH_STACK(stack);

    STACK_VERIFICATION(Stacks, Stack_D, ENOMEM, "%s: In %s:%d: error: Unable to allocate memory.\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

    ((canary_t *)stack->data)[-1]                = Canary_val;
    *(canary_t *)(stack->data + stack->capacity) = Canary_val;

    STACK_DATA_VERIFICATION(Stacks, Stack_D);

#else
    stack->data = (elem_t *)calloc(capacity * sizeof(elem_t), sizeof(char));
#endif

    *stack_descriptor = Stack_D;
    Stack_D++;

    return EXIT_SUCCESS;
}

int stack_dtor(const stk_d stack_descriptor)
{
    STACK_DESCRIPTOR_VERIFICATION(stack_descriptor);

    struct Stack *stack = Stacks + stack_descriptor;

    assert(stack);

    stack->size     = 0;
    stack->capacity = 0;

#ifdef PROTECT
    stack->canary_left  = 0;
    stack->canary_right = 0;

    stack->data_hash  = 0;
    stack->stack_hash = 0;

    stack->err          = {};
    stack->err.invalid  = true;
    stack->err.sizeless = true;
    stack->err.no_data  = true;

    free((canary_t *)stack->data - 1);
#else
    free(stack->data);
#endif

    return EXIT_SUCCESS;
}

int push_stack(const stk_d stack_descriptor, const elem_t val)
{
    struct Stack *stack = Stacks + stack_descriptor;

    VERIFICATION(Stacks, stack_descriptor, EINVAL, "Error: invalid stack.\n"
                                                   "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    int err_code = 0;
    if((err_code = optimal_expansion(stack_descriptor)))
    {
        fprintf(LOG_FILE, "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

        return err_code;
    }

    stack->data[stack->size++] = val;

    HASH_STACK(stack);

    VERIFICATION(Stacks, stack_descriptor, EINVAL, "Error: invalid stack.\n"
                                                   "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    return EXIT_SUCCESS;
}

int pop_stack(const stk_d stack_descriptor, elem_t *ret_val)
{
    struct Stack *stack = Stacks + stack_descriptor;

    VERIFICATION(Stacks, stack_descriptor, EINVAL, "Error: invalid stack.\n"
                                                   "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    if(stack->size == 0)
    {
        fprintf(LOG_FILE, "Error: stack underflow.\n"
                          "%s: In function %s\n", __FILE__, __PRETTY_FUNCTION__);

#ifdef PROTECT
        stack->err.invalid   = true;
        stack->err.underflow = true;
#endif

        HASH_STACK(stack);
        STACK_DUMP(stack_descriptor);

        return EINVAL;
    }

    elem_t value = stack->data[--stack->size];
    stack->data[stack->size] = 0;

    HASH_STACK(stack);

    if(ret_val) *ret_val = value;

    int err_code = 0;
    if((err_code = optimal_shrink(stack_descriptor)))
    {
        fprintf(LOG_FILE, "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);

        return err_code;
    }

    VERIFICATION(Stacks, stack_descriptor, EINVAL, "Error: invalid stack.\n"
                                                   "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    return EXIT_SUCCESS;
}

int optimal_expansion(const stk_d stack_descriptor)
{
    struct Stack *stack = Stacks + stack_descriptor;

    VERIFICATION(Stacks, stack_descriptor, EINVAL, "Error: invalid stack.\n"
                                                   "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    if(stack->size == stack->capacity)
    {
        elem_t *temp_ptr = REALOC_DATA_UP(stack);

        if(!temp_ptr)
        {
            fprintf(LOG_FILE, "Error: unable to reallocate memory.\n"
                              "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 10);

            STACK_DUMP(stack_descriptor);

            return ENOMEM;
        }

        stack->data = temp_ptr;

        for(size_t i = stack->size; i < stack->capacity; i++)
        {
            stack->data[i] = 0;
        }

#ifdef PROTECT
        *(canary_t *)(stack->data + stack->capacity) = Canary_val;
#endif

        HASH_STACK(stack);
    }

    VERIFICATION(Stacks, stack_descriptor, EINVAL, "Error: invalid stack.\n"
                                                   "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    return EXIT_SUCCESS;
}

int optimal_shrink(const stk_d stack_descriptor)
{
    struct Stack *stack = Stacks + stack_descriptor;

    VERIFICATION(Stacks, stack_descriptor, EINVAL, "Error: invalid stack.\n"
                                                   "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    if(stack->size * 4 == stack->capacity)
    {
        elem_t *temp_ptr = REALOC_DATA_DOWN(stack);

        if(!temp_ptr)
        {
            fprintf(LOG_FILE, "Error: unable to reallocate memory.\n"
                              "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 10);

            STACK_DUMP(stack_descriptor);

            return ENOMEM;
        }

        stack->data = temp_ptr;

#ifdef PROTECT
        *(canary_t *)(stack->data + stack->capacity) = Canary_val;
#endif

        HASH_STACK(stack);
    }

    VERIFICATION(Stacks, stack_descriptor, EINVAL, "Error: invalid stack.\n"
                                                   "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);

    return EXIT_SUCCESS;
}

int stack_dump(const stk_d stack_descriptor, const char *Stack_Name, const char *file_name, const char * func_declaration, const int line)
{
    struct Stack *stack = Stacks + stack_descriptor;

    STACK_DESCRIPTOR_VERIFICATION(stack_descriptor);

    assert(stack);
    assert(Stack_Name);
    assert(file_name);
    assert(func_declaration);

    fprintf(LOG_FILE, "Stack[%p] \"%s\" from %s\n"
                      "In function %s:%d\n", stack, Stack_Name, file_name, func_declaration, line);
#ifdef PROTECT
    fprintf(LOG_FILE, "{                     \n"
                      "\tcanary_left = %#llx;\n"
                      "\terr         = %u;   \n"
                      "\tdata_hash   = %zu;  \n"
                      "\tstack_hash  = %zu;  \n", stack->canary_left, *(const unsigned int *)(&stack->err),
                                                  stack->data_hash  , stack->stack_hash);
#endif

    fprintf(LOG_FILE, "\tsize        = %zu;  \n"
                      "\tcapacity    = %zu;  \n"
                      "\tdata[%p]            \n", stack->size, stack->capacity, stack->data);

    if(stack->data != NULL && stack->capacity != 0)
    {
        fprintf(LOG_FILE, "\t{\n");

#ifdef PROTECT
        fprintf(LOG_FILE, "\t\t CANARY_LEFT  = %#llx;\n", ((canary_t *)stack->data)[-1]);
#endif

        for(size_t i = 0; i < stack->capacity; i++)
        {
            fprintf(LOG_FILE, "\t\t");
            fprintf(LOG_FILE, (i < stack->size) ? "*" : " ");
            fprintf(LOG_FILE, "[%3zu] = %lld,\n", i, stack->data[i]);
        }

#ifdef PROTECT
        fprintf(LOG_FILE, "\t\t CANARY_RIGHT = %#llx;\n", *(canary_t *)(stack->data + stack->capacity));
#endif

        fprintf(LOG_FILE, "\t};\n");

#ifdef PROTECT
        fprintf(LOG_FILE, "\tcanary_right = %#llx;\n", stack->canary_right);
#endif
    }
    fprintf(LOG_FILE, "}\n");

    return EXIT_SUCCESS;
}

#ifdef PROTECT
void stack_validation(struct Stack *stack)
{
    assert(stack);

    size_t hash_val = poly_hash_stack(stack);

    stack->err = {};

    stack->err.no_data  = (stack->data == NULL);

    stack->err.sizeless = (stack->capacity == 0);

    stack->err.overflow = (!stack->err.sizeless && stack->size > stack->capacity);

    stack->err.invalid  = (stack->err.no_data || stack->err.sizeless || stack->err.overflow ||
                           stack->canary_left != Canary_val || stack->canary_right != Canary_val ||
                           hash_val != stack->stack_hash);
}

void stack_data_validation(struct Stack *stack)
{
    assert(stack);

    size_t hash_val = poly_hash_data(stack);

    if(((canary_t *)stack->data)[-1] != Canary_val || *(canary_t *)(stack->data + stack->capacity) != Canary_val ||
         hash_val != stack->data_hash)
    {
        stack->err.invalid = true;
    }
}
#endif

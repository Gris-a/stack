#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/stack.h"

static size_t N_buffered_stacks = 1;
struct Stack *Stacks            = NULL;

int stack_ctor(stk_d *stack_descriptor, const size_t capacity)
{
    if(N_buffered_stacks == 1)
    {
        Stacks = (struct Stack *)calloc(++N_buffered_stacks, sizeof(Stack));

        atexit(free_stack_mem);
    }

    static size_t N_stack = 1;

    assert(Stacks);

    assert(stack_descriptor);
    *stack_descriptor = 0;

    if(capacity == 0)
    {
        fprintf(LOG_FILE, "%s: In %s: error: Capasity should be greater than zero.\n", __FILE__, __PRETTY_FUNCTION__);

        return EINVAL;
    }


    if(N_stack == N_buffered_stacks)
    {
        struct Stack *temp_ptr = (struct Stack *)realloc(Stacks, N_buffered_stacks *= 2);

        if(!temp_ptr)
        {
            fprintf(LOG_FILE, "Error: unable to reallocate memory.\n"
                              "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 5);

            return ENOMEM;
        }

        Stacks = temp_ptr;

        for(size_t i = N_buffered_stacks; i < N_buffered_stacks * 2; i++)
        {
            Stacks[i] = {};
        }
    }

    struct Stack *stack = Stacks + N_stack;

    stack->capacity = capacity;

    #ifndef NDEBUG
    stack->canary_left  = Canary_val;
    stack->canary_right = Canary_val;

    stack->data = (elem_t *)((canary_t *)calloc(capacity * sizeof(elem_t) + 2 * sizeof(canary_t), sizeof(char)) + 1);

    HASH_STACK(stack);


    STACK_VERIFICATION(stack, ENOMEM, "%s: In %s:%d: error: Unable to allocate memory.\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2);


    ((canary_t *)stack->data)[-1]                = Canary_val;
    *(canary_t *)(stack->data + stack->capacity) = Canary_val;


    STACK_DATA_VERIFICATION(stack);


    #else
    stack->data = (elem_t *)calloc(capacity * sizeof(elem_t), sizeof(char));
    #endif

    *stack_descriptor = N_stack++;


    return EXIT_SUCCESS;
}

int stack_dtor(const stk_d stack_descriptor)
{
    STACK_DESCRIPTOR_VERIFICATION(stack_descriptor);

    struct Stack *stack = Stacks + stack_descriptor;

    assert(stack);


    stack->size     = 0;
    stack->capacity = 0;

    #ifndef NDEBUG
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
    STACK_DESCRIPTOR_VERIFICATION(stack_descriptor);

    struct Stack *stack = Stacks + stack_descriptor;

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

    HASH_STACK(stack);


    STACK_VERIFICATION(stack, EINVAL, "Error: invalid stack.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 1);
    STACK_DATA_VERIFICATION(stack);


    return EXIT_SUCCESS;
}

int pop_stack(const stk_d stack_descriptor, elem_t *ret_val)
{
    STACK_DESCRIPTOR_VERIFICATION(stack_descriptor);

    struct Stack *stack = Stacks + stack_descriptor;

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

        HASH_STACK(stack);


        return EINVAL;
    }

    elem_t value = stack->data[--stack->size];
    stack->data[stack->size] = 0;

    HASH_STACK(stack);

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
                              "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 10);

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

        HASH_STACK(stack);
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
                              "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 10);

            return ENOMEM;
        }

        stack->data = temp_ptr;

        #ifndef NDEBUG
        *(canary_t *)(stack->data + stack->capacity) = Canary_val;
        #endif

        HASH_STACK(stack);
    }


    STACK_VERIFICATION(stack, ENOMEM, "Error: unable to reallocate memory.\n"
                                      "%s: In function %s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__ - 5);
    STACK_DATA_VERIFICATION(stack);


    return EXIT_SUCCESS;
}

int stack_dump(const stk_d stack_descriptor, const char *stack_name, const char *file_name, const char * func_declaration, const int line)
{
    STACK_DESCRIPTOR_VERIFICATION(stack_descriptor);

    struct Stack *stack = Stacks + stack_descriptor;

    assert(stack);
    assert(stack_name);
    assert(file_name);
    assert(func_declaration);


    fprintf(LOG_FILE, "Stack[%p] \"%s\" from %s\n"
                      "In function %s:%d\n", stack, stack_name, file_name, func_declaration, line);
    #ifndef NDEBUG
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

        #ifndef NDEBUG
        fprintf(LOG_FILE, "\t\t CANARY_LEFT  = %#llx;\n", ((canary_t *)stack->data)[-1]);
        #endif

        for(size_t i = 0; i < stack->capacity; i++)
        {
            fprintf(LOG_FILE, "\t\t");
            fprintf(LOG_FILE, (i < stack->size) ? "*" : " ");
            fprintf(LOG_FILE, "[%3zu] = %lld,\n", i, stack->data[i]);
        }

        #ifndef NDEBUG
        fprintf(LOG_FILE, "\t\t CANARY_RIGHT = %#llx;\n", *(canary_t *)(stack->data + stack->capacity));
        #endif

        fprintf(LOG_FILE, "\t};\n");

        #ifndef NDEBUG
        fprintf(LOG_FILE, "\tcanary_right = %#llx;\n", stack->canary_right);
        #endif
    }
    fprintf(LOG_FILE, "}\n");


    return EXIT_SUCCESS;
}

void free_stack_mem(void)
{
    free(Stacks);
}

#ifndef NDEBUG
void stack_validation(struct Stack *stack)
{
    assert(stack);


    size_t hash_to_check = stack->stack_hash;
    poly_hash_stack(stack, &stack->stack_hash);

    stack->err = {};

    stack->err.no_data  = (stack->data == NULL);

    stack->err.sizeless = (stack->capacity == 0);

    stack->err.overflow = (!stack->err.sizeless && stack->size > stack->capacity);

    stack->err.invalid  = (stack->err.no_data  || stack->err.sizeless || stack->err.overflow ||
                           stack->canary_left != Canary_val || stack->canary_right != Canary_val || hash_to_check != stack->stack_hash);

    stack->stack_hash = hash_to_check;
}

void stack_data_validation(struct Stack *stack)
{
    assert(stack);


    size_t hash_val = 0;
    poly_hash_data(stack, &hash_val);

    if(((canary_t *)stack->data)[-1] != Canary_val || *(canary_t *)(stack->data + stack->capacity) != Canary_val ||
         hash_val != stack->data_hash)
    {
        stack->err.invalid = true;
    }
}
#endif
#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

typedef size_t stk_d;
typedef long long elem_t;
typedef unsigned long long canary_t;

const canary_t Canary_val = 0xB1BAB0BA;

struct Err
{
    unsigned int invalid  :1;
    unsigned int no_data  :1;
    unsigned int sizeless :1;
    unsigned int overflow :1;
    unsigned int underflow:1;
};

struct Stack
{
    #ifndef NDEBUG
    canary_t canary_left;
    #endif

    size_t size;
    size_t capacity;

    elem_t *data;

    #ifndef NDEBUG
    size_t stack_hash;
    size_t data_hash;

    struct Err err;

    canary_t canary_right;
    #endif
};

#endif //TYPES_H
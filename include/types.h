#ifndef TYPES_H
#define TYPES_H

#define PROTECT

/**
 * @file types.h
 * @author GraY
 * @brief Defines types for @b Stack structure.
 */

#include <stddef.h>

typedef size_t stk_d;                ///< Type define for @b stack descriptor.
typedef long long elem_t;            ///< Type define for elements of @b Stack data.
#define ETS "%lld"

#ifdef PROTECT

typedef unsigned long long canary_t; ///< Type define for @b Canary value.
const canary_t Canary_val = 0xB1BAB0BA; ///< Canary value for canary protection.

/**
 * @brief Errors bit-field.
 * Shows errors in @b Stack structure.
 */
struct Err
{
    unsigned int invalid  :1;
    unsigned int no_data  :1;
    unsigned int sizeless :1;
    unsigned int overflow :1;
    unsigned int underflow:1;
};

#endif

/**
 * @brief Stack structure.
 */
struct Stack
{
    #ifdef PROTECT
    canary_t canary_left;  ///< Left @b Canary for canary protection. Field doesn`t creates if defined @b NDEBUG.
    #endif

    size_t size;           ///< Number of elements in @b Stack data.
    size_t capacity;       ///< Number of max amount of elements in @b Stack data.

    elem_t *data;          ///< @b Stack data.

    #ifdef PROTECT
    size_t stack_hash;     ///< Hashed @b Stack value. Field doesn`t creates if defined @b NDEBUG.
    size_t data_hash;      ///< Hashed @b Stack data. Field doesn`t creates if defined @b NDEBUG.

    struct Err err;        ///< Errors bit-field. Field doesn`t creates if defined @b NDEBUG.

    canary_t canary_right; ///< Right @b Canary for canary protection. Field doesn`t creates if defined @b NDEBUG.
    #endif
};

#endif //TYPES_H
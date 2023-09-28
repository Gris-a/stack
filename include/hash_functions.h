#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

/**
 * @file hash_functions.h
 * @author GraY
 * @brief Hash-Functions for @b Stack and it`s @b data.
 */

#include <stddef.h>

#include "types.h"

/**
 * @brief Function hashes @b Stack data and returns hash value.
 * @param stack Pointer to the @b Stack structure.
 * @return size_t hash value of @b Stack data.
 */
size_t poly_hash_data(Stack *stack);

/**
 * @brief Function hashes @b Stack structure and returns hash value.
 * @param stack Pointer to the @b Stack structure.
 * @return size_t hash value of @b Stack structure.
 */
size_t poly_hash_stack(Stack *stack);

#endif //HASH_FUNCTIONS_H

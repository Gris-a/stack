#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stddef.h>

#include "stack.h"

const unsigned long long P = 257;

void poly_hash_data(Stack *stack, size_t *hash);

void poly_hash_stack(Stack *stack, size_t *hash);

#endif //HASH_FUNCTIONS_H

#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stddef.h>

#include "stack.h"

const size_t P = 257;

void poly_hash(const Stack *stack, size_t *hash);

#endif //HASH_FUNCTIONS_H

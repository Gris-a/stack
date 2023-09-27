#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stddef.h>

#include "types.h"

size_t poly_hash_data(Stack *stack);

size_t poly_hash_stack(Stack *stack);

#endif //HASH_FUNCTIONS_H

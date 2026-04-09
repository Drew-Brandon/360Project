#ifndef BIT_UTILS_H
#define BIT_UTILS_H
#include <inttypes.h>
#include "boolean.h"

/// @brief A convenience type for representing variables that are only a single byte.
typedef uint8_t byte;

boolean bit_is_marked(const byte loc, unsigned int bit_num);
#endif

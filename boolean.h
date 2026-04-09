#ifndef BOOLEAN_H
#define BOOLEAN_H
#include <inttypes.h>

/// @brief A convenience macro for getting a false boolean value (0).
#define FALSE (boolean)0

/// @brief A convenience macro for getting a true boolean value (1).
#define TRUE (boolean)1

/// @brief A convenience type for representing boolean values as unsigned 8-bit integers.
typedef uint8_t boolean;
#endif

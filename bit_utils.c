#include "bit_utils.h"

boolean bit_is_marked(const byte loc, unsigned int bit_num)
{
    return loc | (bit_num << 1);
}

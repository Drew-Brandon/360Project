#ifndef DEBUG_H
#define DEBUG_H
#define IN_DEBUG_MODE true
#define DEBUG1_true
#define DEBUG2_true
#define DEBUG1_false /
#define DEBUG2_false *
#define DEBUG_SHELL(content, in_debug_mode) \
DEBUG1_##in_debug_modeDEBUG2_##in_debug_mode \
content \
DEBUG2_##in_debug_mode DEBUG1_##in_debug_mode

#endif DEBUG_H

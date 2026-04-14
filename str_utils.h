#ifndef STR_UTILS_H
#define STR_UTILS_H
#include "array_list.h"

DEF_ARRAY_LIST_HEADER(char, Char, ch)

DEF_ARRAY_LIST_HEADER(ArrayListChar, String, str)

ArrayListString split_str(const char *str, const char split_ch);

#endif

#ifndef STR_UTILS_H
#define STR_UTILS_H
#include "array_list.h"

typedef struct
{
    int length;
    char *str;
} Line;

DEF_ARRAY_LIST_HEADER(char, Char, ch)
DEF_ARRAY_LIST_HEADER(char *, String, str)
DEF_ARRAY_LIST_HEADER(Line, Line, line)

void free_str_list(ArrayListString *list);

int star_compare(const char *search_term, const char *searched_text);

ArrayListString split_str(char *str, char delimiter);
#endif

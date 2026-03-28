#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H
#include <stdlib.h>
#define MIN_CAPACITY 8

#define DEF_ARRAY_LIST_TYPE(type, upper_name) \
typedef struct \
{ \
    int length; \
    int capacity; \
    type *arr; \
} ArrayList##upper_name;

#define DEF_ARRAY_LIST_HEADER(type, upper_name, lower_name) \
DEF_ARRAY_LIST_TYPE(type, upper_name) \
\
void init_arr_list_##lower_name(ArrayList##upper_name *list); \
void push_arr_list_##lower_name(ArrayList##upper_name *list, type val); \
void insert_at_arr_list_##lower_name(ArrayList##upper_name *list, int index, type val); \
void remove_at_arr_list_##lower_name(ArrayList##upper_name *list, int index);

#define DEF_ARRAY_LIST_SOURCE(type, upper_name, lower_name) \
void init_arr_list_##lower_name(ArrayList##upper_name *list) \
{ \
    list->length = 0; \
    list->capacity = MIN_CAPACITY; \
    list->arr = (type *)malloc(list->capacity * sizeof(type)); \
} \
\
void push_arr_list_##lower_name(ArrayList##upper_name *list, type val) \
{ \
    if (list->length >= list->capacity) \
    { \
        list->capacity <<= 1; \
        list->arr = (type *)realloc(list->arr, list->capacity * sizeof(type)); \
    } \
    \
    list->arr[list->length] = val; \
    list->length++; \
} \
\
void insert_at_arr_list_##lower_name(ArrayList##upper_name *list, int index, type val) \
{ \
    if (list->length >= list->capacity) \
    { \
        list->capacity <<= 1; \
        list->arr = (type *)realloc(list->arr, list->capacity * sizeof(type)); \
    } \
    \
    for (int i = list->length; i > index; i--) \
    { \
        list->arr[i] = list->arr[i - 1]; \
    } \
    \
    list->arr[index] = val; \
    list->length++; \
} \
\
void remove_at_arr_list_##lower_name(ArrayList##upper_name *list, int index) \
{ \
    list->length--; \
    \
    for (int i = index; i < list->length; i++) \
    { \
        list->arr[i] = list->arr[i + 1]; \
    } \
    \
    if (list->length <= list->capacity >> 1) \
    { \
        list->capacity >>= 1; \
        list->arr = (type *)realloc(list->arr, list->capacity * sizeof(type)); \
    } \
} \
\
void pack_arr_list_##lower_name(ArrayList##upper_name *list) \
{ \
    list->capacity = list->length; \
    list->arr = (type *)realloc(list->arr, list->capacity * sizeof(type)); \
}

#define DEF_ARRAY_LIST_FULL(type, upper_name, lower_name) \
DEF_ARRAY_LIST_TYPE(type, upper_name) \
DEF_ARRAY_LIST_SOURCE(type, upper_name, lower_name) \

#endif

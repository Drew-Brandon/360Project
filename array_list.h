#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H
#include <stdlib.h>
#include "boolean.h"
#define MIN_CAPACITY 8

/// @brief Defines the structure for an array list of the specified type.
/// @param type The type of values to store in the list.
/// @param pascal_name The pascal case name to append to the structure.
#define DEF_ARRAY_LIST_TYPE(type, pascal_name) \
typedef struct \
{ \
    int length; \
    int capacity; \
    type *arr; \
} ArrayList##pascal_name;

/// @brief Defines the header code for an array. This should be used in header files.
/// @param type The type of values to store in the list.
/// @param pascal_name The pascal case name to append to the structure.
/// @param snake_name The snake case name to append to the functions.
#define DEF_ARRAY_LIST_HEADER(type, pascal_name, snake_name) \
DEF_ARRAY_LIST_TYPE(type, pascal_name) \
\
void init_arr_list_##snake_name(ArrayList##pascal_name *list); \
void push_arr_list_##snake_name(ArrayList##pascal_name *list, type val); \
void push_null_arr_list_##snake_name(ArrayList##pascal_name *list); \
void insert_at_arr_list_##snake_name(ArrayList##pascal_name *list, int index, type val); \
void pop_arr_list_##snake_name(ArrayList##pascal_name *list); \
void remove_at_arr_list_##snake_name(ArrayList##pascal_name *list, int index); \
void pack_arr_list_##snake_name(ArrayList##pascal_name *list);

/// @brief Defines only the source code for an array. This should be used in source code files.
// Note that the list's structure is not defined in this function.
/// @param type The type of values to store in the list.
/// @param pascal_name The pascal case name to append to the structure.
/// @param snake_name The snake case name to append to the functions.
#define DEF_ARRAY_LIST_SOURCE(type, pascal_name, snake_name) \
void init_arr_list_##snake_name(ArrayList##pascal_name *list) \
{ \
    list->length = 0; \
    list->capacity = MIN_CAPACITY; \
    list->arr = (type *)malloc(list->capacity * sizeof(type)); \
} \
\
void push_arr_list_##snake_name(ArrayList##pascal_name *list, type val) \
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
void push_null_arr_list_##snake_name(ArrayList##pascal_name *list) \
{ \
    if (list->length >= list->capacity) \
    { \
        list->capacity <<= 1; \
        list->arr = (type *)realloc(list->arr, list->capacity * sizeof(type)); \
    } \
    \
    list->length++; \
} \
\
void insert_at_arr_list_##snake_name(ArrayList##pascal_name *list, int index, type val) \
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
void pop_arr_list_##snake_name(ArrayList##pascal_name *list) \
{ \
    list->length--; \
    \
    if (list->length <= list->capacity >> 1 && list->capacity > MIN_CAPACITY) \
    { \
        list->capacity >>= 1; \
        list->arr = (type *)realloc(list->arr, list->capacity * sizeof(type)); \
    } \
} \
\
void remove_at_arr_list_##snake_name(ArrayList##pascal_name *list, int index) \
{ \
    list->length--; \
    \
    for (int i = index; i < list->length; i++) \
    { \
        list->arr[i] = list->arr[i + 1]; \
    } \
    \
    if (list->length <= list->capacity >> 1 && list->capacity > MIN_CAPACITY) \
    { \
        list->capacity >>= 1; \
        list->arr = (type *)realloc(list->arr, list->capacity * sizeof(type)); \
    } \
} \
\
void pack_arr_list_##snake_name(ArrayList##pascal_name *list) \
{ \
    list->capacity = list->length; \
    list->arr = (type *)realloc(list->arr, list->capacity * sizeof(type)); \
}

/// @brief Defines the entire code for an array. This should be used in source code files.
/// This is meant to be utilized when no header info for the list was defined,
/// and will include define the structure.
/// @param type The type of values to store in the list.
/// @param pascal_name The pascal case name to append to the structure.
/// @param snake_name The snake case name to append to the functions.
#define DEF_ARRAY_LIST_FULL(type, pascal_name, snake_name) \
DEF_ARRAY_LIST_TYPE(type, pascal_name) \
DEF_ARRAY_LIST_SOURCE(type, pascal_name, snake_name)
#endif

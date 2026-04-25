#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H
#include "boolean.h"

/// @brief Represents the result of a search through an array.
typedef struct
{
    boolean found;
    int index;
} SearchResult;

/// @brief Defines the header for a binary search function of the specified types.
/// @param arr_type The type of array to search through.
/// @param targ_type The type of value to search for.
/// @param snake_name The snake case name to append to this function.
#define DEF_BINARY_SEARCH_HEADER(arr_type, targ_type, snake_name) \
SearchResult binary_search_##snake_name(int start, int end, arr_type *arr, targ_type target, int (*cmp)(const arr_type, const targ_type));

/// @brief Defines the source for a binary search function of the specified types.
/// @param arr_type The type of array to search through.
/// @param targ_type The type of value to search for.
/// @param snake_name The snake case name to append to this function.
#define DEF_BINARY_SEARCH_SOURCE(arr_type, targ_type, snake_name) \
SearchResult binary_search_##snake_name(int start, int end, arr_type *arr, targ_type target, int (*cmp)(const arr_type, const targ_type)) \
{ \
    int mid = start; \
    \
    while (start <= end) \
    { \
        mid = start + (end - start) / 2; \
        int cmp_result = cmp(arr[mid], target); \
        \
        if (cmp_result == 0) \
        { \
            return (SearchResult){TRUE, mid}; \
        } \
        else if (cmp_result > 0) \
        { \
            end = mid - 1; \
            mid = start; \
        } \
        else \
        { \
            start = mid + 1; \
            mid = end + 1; \
        } \
    } \
    \
    return (SearchResult){FALSE, mid}; \
}
#endif

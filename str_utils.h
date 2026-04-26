#ifndef STR_UTILS_H
#define STR_UTILS_H
#include "array_list.h"
#include "boolean.h"

/// @brief Represents a string of a specific length.
typedef struct
{
    int length;
    char *str;
} Line;

DEF_ARRAY_LIST_HEADER(char, Char, ch)
DEF_ARRAY_LIST_HEADER(char *, String, str)
DEF_ARRAY_LIST_HEADER(Line, Line, line)

/// @brief Frees the given list of strings.
/// @arg The lis to free.
void free_str_list(ArrayListString *list);

/// @brief Compares the given text to the search term.
/// @param search_term The term to search for.
/// @param searched_text The text to search through.
/// @return Whether or not the text matches the given search term.
boolean star_compare(const char *search_term, const char *searched_text);

/// @brief Splits the given string into multiple chunks seperated by the specified delimiter.
/// @param str The string to split.
/// @param delimiter The delimiter to split with.
/// @return The split substrings taken from the given string.
ArrayListString split_str(char *str, char delimiter);

/// @brief Copies the given string into an array list.
/// @param str The string to copy.
/// @return The list copied into.
ArrayListChar copy_str_till(char *str, char stop);
#endif

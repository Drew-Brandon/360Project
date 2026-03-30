#ifndef FILE_UTILS_H
#define FILE_UTILS_H
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "array_list.h"
#define MAX_LINE_LENGTH 512

/// @brief Stores the name, line count, and lines of a file.
typedef struct
{
    int nlines;
    char *name;
    char **lines;
} EZFile;

/// @brief Indents a line by a certain amount with tabs.
/// @param ez_file The file to indent within.
/// @param layer The amount of tabs to indent with.
void indent_line(FILE *ez_file, int layer);

/// @brief Skips the specified amount of indents/tabs at the start of a line.
/// @param ez_file The file to skip within.
/// @param layer The amount of indents/tabs to skip.
void skip_indents(FILE *ez_file, int layer);

/// @brief Frees up the specified EZFile structure.
/// @param file The structure to free.
void free_ez_file(EZFile *file);

/// @brief Opens a directory and prints the names of all files and subdirectories inside it
/// @param path the directory path
void list_directory(const char *path);

/// @brief Opens a file and prints its contents to the terminal line by line
/// @param path A file to be read
void display_file(const char *path);

/// @brief determines whether the argument is a file or a directory
/// @param path 
void handle_path(const char *path);
#endif

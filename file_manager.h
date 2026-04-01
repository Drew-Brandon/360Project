#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

/// @brief Opens a directory and prints the names of all files and subdirectories inside it
/// @param path the directory path
void list_directory(const char *path);

/// @brief Opens a file and prints its contents to the terminal line by line
/// @param path A file to be read
void display_file(const char *path);

/// @brief determines whether the argument is a file or a directory
/// @param path 
void handle_path(const char *path);

/// @brief Handles a command by parsing it and executing the appropriate function.
/// @param input The command to be handled
void handle_command(char *input);
#endif 
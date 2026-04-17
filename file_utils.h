#ifndef FILE_UTILS_H
#define FILE_UTILS_H
#include <stdio.h>
#include "array_list.h"

#define OBJ_TYPE_SIZE 5

/// @brief The tag that is used to represent an object being a directory in a .ez file.
#define DIR_TAG "diry"

/// @brief The tag that is used to represent an object being a file in a .ez file.
#define FILE_TAG "file"

#define MAX_FILE_NAME_SIZE 256
#define MAX_LINE_SIZE 512
#define MAX_CH_COUNT (sizeof(char) * 8) << 1
#define MAX_CH_COUNT_BYTES MAX_CH_COUNT / 8

DEF_ARRAY_LIST_HEADER(char, Char, ch)

DEF_ARRAY_LIST_HEADER(ArrayListChar, String, str)

/// @brief An enum representing the types that an object can be in a .ez file.
enum EZObjectType
{
    OBJ_NONE = 0,
    OBJ_DIRECTORY = 1,
    OBJ_FILE = 2
};

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

/// @brief Returns the next character in the file.
/// @param file The file to peek in.
/// @return The next character in the file.
char peek_char(FILE *file);

/// @brief Skips to the specified character in the file.
/// @param file The file to skip in.
/// @param ch The character to skip to.
void skip_to_char(FILE *file, char ch);

/// @brief Skips the whitespace in the file.
/// @param file The file to skip in.
void skip_whitespace(FILE *file);

/// @brief Scans lines until the specified character is reached.
/// @param file The file to scan through.
/// @param ch The character to stop scanning at.
/// @param layer The layer to scan in. This refers to the amount of tab indents should be present for the text.
/// @return The lines that were scanned in.
ArrayListString scan_till_char(FILE *file, char ch, int layer);

/// @brief Scans the header for an object in a .ez file.
/// @param file The file to scan the header from.
/// @param obj_name_size The maximum size the object's name can be.
/// @param obj_name The string to store the name of the object in.
/// @return The type of object that was scanned.
enum EZObjectType scan_header(FILE *file, int obj_name_size, char *obj_name);

/// @brief Frees up the specified EZFile structure.
/// @param file The structure to free.
void free_ez_file(EZFile *file);
/// @brief  Uploads a file to a .ez file with the specified virtual name and indentation layer.
/// @param ez_out The .ez file to upload to.
/// @param real_path The path to the file to upload.
/// @param virtual_name The name to give the file within the .ez file. This is not required to be the same as the real name of the file, but it must be unique within the .ez file. It also cannot contain any spaces or tabs.
/// @param layer The indentation layer to upload the file at. This refers to the amount of tab indents that should be before the file's header in the .ez file.
void upload_file(FILE *ez_out, const char *real_path, const char *virtual_name, int layer);

/// @brief Uploads a directory to a .ez file with the specified indentation layer. The directory is uploaded recursively, so all subdirectories and files within the directory are also uploaded.
/// @param ez_out The .ez file to upload to.
/// @param path The path to the directory to upload.
/// @param layer The indentation layer to upload the directory at. This refers to the amount of tab indents that should be before the directory's header in the .ez file.
void upload_directory(FILE *ez_out, const char *path, int layer);


#endif


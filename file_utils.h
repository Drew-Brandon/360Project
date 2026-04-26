#ifndef FILE_UTILS_H
#define FILE_UTILS_H
#include <stdio.h>
#include "str_utils.h"
#include "array_list.h"
#define OBJ_TYPE_SIZE 5

/// @brief The tag that is used to represent an object being a directory in a .ez file.
#define DIR_TAG "diry"

/// @brief The tag that is used to represent an object being a file in a .ez file.
#define FILE_TAG "file"

/// @brief The maximum size a file name can be.
#define MAX_OBJ_NAME_SIZE 256

/// @brief An enum representing the types that an object can be in a .ez file.
enum EZObjectType
{
    OBJ_NONE = 0,
    OBJ_FILE = 1,
    OBJ_DIRECTORY = 2,
};

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
ArrayListLine scan_till_char(FILE *file, char ch, int layer);

/// @brief Scans the header for an object in a .ez file.
/// @param file The file to scan the header from.
/// @param obj_name_size The maximum size the object's name can be.
/// @param obj_name The string to store the name of the object in.
/// @return The type of object that was scanned.
enum EZObjectType scan_header(FILE *file, int obj_name_size, char *obj_name);

// VFS tree building
struct VNode;  // forward declaration

/// @brief Uploads the specified file to the system.
/// @param in The file to upload.
/// @param fname The name of the file to upload.
/// @param parent The parent directory to upload the file to.
/// @return The node created for the file.
struct VNode *upload_file(FILE *in, const char *fname, struct VNode *parent);

/// @brief Builds a VFS tree from a .ez file.
/// @param in The file to build the tree from.
/// @return The root of the built tree.
struct VNode *build_ez_tree(FILE *in);

/// @brief Scans a directory node and its children from a .ez file.
/// @param in The file to scan the node from.
/// @param name The name of the directory.
/// @param parent The parent of the directory. Can be NULL if this is the root directory.
/// @return The scanned directory node.
struct VNode *scan_dir_node(FILE *in, char *name, struct VNode *parent, int layer);

/// @brief Scans a file node from a .ez file.
/// @param in The file to scan the node from.
/// @param name The name of the file.
/// @param parent The parent of the file. Can be NULL if this is the root directory
/// @return The scanned file node.
struct VNode *scan_file_node(FILE *in, char *name, struct VNode *parent, int layer);

/// @brief Prints out the specified directory node to the specified file.
/// @param out The file to print to.
/// @param node The node to print out.
/// @param layer The layer of indentation to apply to the printing.
void print_dir_node(FILE *out, struct VNode *node, int layer);

/// @brief Prints out the specified file node to the specified file.
/// @param out The file to print to.
/// @param node The node to print out.
/// @param layer The layer of indentation to apply to the printing.
void print_file_node(FILE *out, struct VNode *node, int layer);

#endif

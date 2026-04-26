#ifndef VFS_H
#define VFS_H

#include "file_utils.h"

typedef struct VNode VNode;

DEF_ARRAY_LIST_HEADER(VNode*, VNodePtr, vnode_ptr)
DEF_ARRAY_LIST_HEADER(char*, CharPtr, ch_ptr)

struct VNode
{
    char name[MAX_FILE_NAME_SIZE];
    enum EZObjectType type;
    VNode *parent;

    union
    {
        struct
        {
            ArrayListChar *lines;
            int nlines;
        } file;

        struct
        {
            ArrayListVNodePtr children;
        } dir;
    };
};

typedef struct 
{
    ArrayListCharPtr commands;
} History;

typedef struct
{
    VNode *root;
    VNode *cwd;
} VFS;

/// @brief Creates a directory node with the specified name and parent.
/// @param name The name of the directory. 
/// @param parent The parent of the directory. Can be NULL if this is the root directory.
/// @return The created directory node.
VNode *create_dir(char *name, VNode *parent);

/// @brief  Creates a file node with the specified name and parent.
/// @param name The name of the file.
/// @param parent The parent of the file. Can be NULL if this is the root directory.
/// @return The created file node.
VNode *create_file(char *name, VNode *parent);

/// @brief Lists the contents of the current directory.
/// @param cwd The current directory to list the contents of.
void cmd_ls(VNode *cwd);

/// @brief Changes the current directory to the specified path if it exists.
/// @param vfs The virtual file system to change the directory in.
/// @param path The name of the directory to change to. Can be ".." to go up a directory.
void cmd_cd(VFS *vfs, const char *path);

/// @brief Prints the contents of the specified file.
/// @param cwd The current directory to look for the file in.
/// @param name The name of the file to print.
void cmd_cat(VNode *cwd, const char *name);

/// @brief Prints the path of the specified node from the root.
/// @param node The node to print the path of.
void print_path(VNode *node);

/// @brief  Runs a simple shell that allows the user to navigate the virtual file system and print file contents.
/// @param vfs The virtual file system to run the shell on.
void run_shell(VFS *vfs);

/// @brief Frees the memory used by a vnode and all of its children if it is a directory.
/// @param node The vnode to free.
void free_vnode(VNode *node);

#endif
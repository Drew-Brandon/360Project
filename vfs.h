#ifndef VFS_H
#define VFS_H

#include "file_utils.h"
#include "binary_search.h"

typedef struct VNode VNode;

DEF_ARRAY_LIST_HEADER(VNode*, VNodePtr, vnode_ptr)

/// @brief Represents a node in a file system that can be a directory or file.
struct VNode
{
    char name[MAX_OBJ_NAME_SIZE];
    enum EZObjectType type;
    VNode *parent;

    union
    {
        struct
        {
            Line *lines;
            int nlines;
        } file;

        struct
        {
            ArrayListVNodePtr children;
            int dir_count;
        } dir;
    };
};

/// @brief Represents a virtual file system with a root and current working directory.
typedef struct
{
    VNode *root;
    VNode *cwd;
} VFS;

/// @brief Inserts the specified directory into the directory 
/// @param dir The directory to insert into the given parent directory.
/// @param parent The parent to attach the file to.
/// @return Whether or not the insertion was a failure.
boolean insert_dir_into_dir(VNode *dir, VNode *parent);

/// @brief Inserts the specified file into the directory 
/// @param file The file to insert into the directory.
/// @param parent The parent to attach the file to.
/// @return Whether or not the insertion was a failure.
boolean insert_file_into_dir(VNode *file, VNode *parent);

/// @brief Creates a directory node with the specified name and parent.
/// @param name The name of the directory. 
/// @param parent The parent of the directory. Can be NULL if this is the root directory.
/// @return The created directory node.
VNode *create_dir(const char *name, VNode *parent);

/// @brief  Creates a file node with the specified name and parent.
/// @param name The name of the file.
/// @param parent The parent of the file. Can be NULL if this is the root directory.
/// @return The created file node.
VNode *create_file(const char *name, VNode *parent);

/// @brief  Runs a simple shell that allows the user to navigate the virtual file system and print file contents.
/// @param vfs The virtual file system to run the shell on.
void run_shell(VFS *vfs);

/// @brief Frees the memory used by a vnode and all of its children if it is a directory.
/// @param node The vnode to free.
void free_vnode(VNode *node);
#endif

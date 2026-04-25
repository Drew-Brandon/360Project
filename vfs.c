#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"
#include "journaling.h"
#include "boolean.h"

DEF_ARRAY_LIST_SOURCE(VNode *, VNodePtr, vnode_ptr)
DEF_BINARY_SEARCH_SOURCE(VNode *, VNode *, vnode_ptr)
DEF_BINARY_SEARCH_SOURCE(VNode *, char *, vnode_ptr_name)

/// @brief Compares the two nodes to each other.
/// Note that nodes are ordered alphabetically.
/// @param n1 The first node to compare.
/// @param n2 The second node to compare.
/// @return 0 if both nodes are equal (of the same type and name).
/// Greater than 0 if the second node comes after.
/// Less than 0 if the second node comes before.
int cmp_node(const VNode *n1, const VNode *n2)
{
    return strcmp(n1->name, n2->name);
}

/// @brief Compares the node's name to the given string.
/// @param node The node to compare.
/// @param name The string/name to compare.
/// @return 0 if the name of the node is the same as the given string.
/// Greater than 0 if the node's name comes after the given one.
/// Less than 0 if the node's name comes before the given one.
int cmp_node_name(const VNode *node, const char *name)
{
    return strcmp(node->name, name);
}

boolean insert_dir_into_dir(VNode *dir, VNode *parent)
{
    SearchResult result = binary_search_vnode_ptr(0, parent->dir.dir_count - 1, parent->dir.children.arr, dir, cmp_node);

    if (result.found)
    {
        printf("Cannot insert the directory of the name \"%s\" into the directory named \"%s\" as that directory already exists within it", dir->name, parent->name);
        return TRUE;
    }

    insert_at_arr_list_vnode_ptr(&parent->dir.children, result.index, dir);
    return FALSE;
}

boolean insert_file_into_dir(VNode *file, VNode *parent)
{
    SearchResult result = binary_search_vnode_ptr(parent->dir.dir_count, parent->dir.children.length - 1, parent->dir.children.arr, file, cmp_node);
    
    if (result.found)
    {
        printf("Cannot insert the file of the name \"%s\" into the directory named \"%s\" as that file already exists within it", file->name, parent->name);
        return TRUE;
    }

    insert_at_arr_list_vnode_ptr(&parent->dir.children, result.index, file);
    return FALSE;
}

/// @brief A method that triggers the program to scan for a yes or no query.
/// @return A boolean value representing whether or not the input was yes.
boolean confirm_input()
{
    char input[3];

    do
    {
        fgets(input, sizeof(input), stdin);
    } while(input[0] != 'y' && input[0] != 'n');

    return (boolean)(input[0] == 'y');
}

VNode *create_dir(const char *name, VNode *parent)
{
    VNode *node = malloc(sizeof(VNode));

    if (!node)
    {
        THROW_FMT("Faile to allocate memory for the directory named \"%s\"", name);
    }

    // Set basic info.
    strcpy(node->name, name);
    node->type = OBJ_DIRECTORY;
    node->parent = parent;

    // Initialize directory-specific data.
    init_arr_list_vnode_ptr(&node->dir.children);

    return node;
}

VNode *create_file(const char *name, VNode *parent)
{
    VNode *node = (VNode *)malloc(sizeof(VNode));

    if (!node)
    {
        THROW_FMT("Faile to allocate memory for the file named \"%s\"", name);
    }

    // Set basic info.
    strncpy(node->name, name, MAX_OBJ_NAME_SIZE - 1);
    node->name[MAX_OBJ_NAME_SIZE - 1] = '\0';

    node->type = OBJ_FILE;
    node->parent = parent;

    // Initialize file-specific data.
    node->file.lines = NULL;
    node->file.nlines = 0;

    return node;
}

/// @brief Lists out the contents of the current working directory.
/// @param cwd The current working directory to lis the contents on.
void cmd_ls(VNode *cwd)
{
    if (cwd->type != OBJ_DIRECTORY)
    {
        printf("Not a directory\n");
        return;
    }

    for (int i = 0; i < cwd->dir.children.length; i++)
    {
        VNode *child = cwd->dir.children.arr[i];  

        printf("%s%s\n",
            child->name,
            child->type == OBJ_DIRECTORY ? "/" : "");
    }
}

/// @brief Changes the directory to what the specified path is pointing to.
/// @param vfs The virtual file system being operated in.
/// @param path The path pointing to the directory to change to.
void cmd_cd(VFS *vfs, const char *path)
{
    if (!vfs || !path) return;

    // Handle root
    if (strcmp(path, "/") == 0)
    {
        vfs->cwd = vfs->root;
        return;
    }

    // Handle parent
    if (strcmp(path, "..") == 0)
    {
        if (vfs->cwd->parent != NULL)
        {
            vfs->cwd = vfs->cwd->parent;
        }
        return;
    }

    // Search for child directory
    for (int i = 0; i < vfs->cwd->dir.children.length; i++)
    {
        VNode *child = vfs->cwd->dir.children.arr[i]; 

        if (child->type == OBJ_DIRECTORY &&
            strcmp(child->name, path) == 0)
        {
            vfs->cwd = child;
            return;
        }
    }

    printf("cd: %s: No such directory\n", path);
}

/// @brief Displays the contents of a file.
/// @param cwd The directory that the file is in.
/// @param name The name of the file to display.
void cmd_cat(VNode *cwd, const char *name)
{
    if (cwd->type != OBJ_DIRECTORY)
    {
        printf("Not a directory\n");
        return;
    }

    for (int i = 0; i < cwd->dir.children.length; i++)
    {
        VNode *child = cwd->dir.children.arr[i];

        if (child->type == OBJ_FILE &&
            strcmp(child->name, name) == 0)
        {
            // Print file contents cleanly
            for (int j = 0; j < child->file.nlines; j++)
            {
                if (child->file.lines[j].arr != NULL)
                {
                    printf("%s\n", child->file.lines[j].arr);
                }
            }
            return;
        }
    }

    printf("cat: %s: No such file\n", name);
}

/// @brief Prints out the current working directory alongside its path.
/// @param node The directory to display.
void cmd_pwd(VNode *node)
{
    if (node->parent != NULL)
    {
        cmd_pwd(node->parent);
    }

    printf("/%s", node->name);
}

/// @brief Saves the file system to the specified file on the local computer.
/// @param vfs The virtual system to save.
/// @param save_file The file to save to.
void cmd_save(VFS *vfs, const char *save_file)
{
    FILE *file = fopen(save_file, "w");

    if (!file)
    {
        THROW_FMT("Couldn't open file at location \"%s\"", save_file);
    }

    print_dir_node(file, vfs->root, 0);
    fclose(file);
}

/// @brief Uploads the specified file on the local computer to the current directory.
/// @param parent The parent to upload to.
/// @param fname The name of the file to upload.
void cmd_upload(VNode *parent, const char *fname)
{
    FILE *file = fopen(fname, "r");

    if (!file)
    {
        THROW_FMT("Couldn't open file at location \"%s\"", file);
    }

    VNode *node = upload_file(file, fname, parent);
    fclose(file);
    insert_file_into_dir(parent, node);
}

void cmd_upload_dir(VNode *parent, const char *fname)
{
    FILE *file = fopen(fname, "r");

    if (!file)
    {
        THROW_FMT("Couldn't open file at location \"%s\"", file);
    }

    VNode *node = upload_file(file, fname, parent);
    fclose(file);
    insert_dir_into_dir(parent, node);
}

/// @brief Creates the directory and attaches it to the specified parent.
/// @param parent The parent of the directory.
/// @param dir_name The name of the directory.
void cmd_mkdir(VNode *parent, const char *dir_name)
{
    VNode *node = create_dir(dir_name, parent);
    insert_dir_into_dir(parent, node);
}

/// @brief Removes the specified file from the system.
/// @param parent The parent of the file to remove.
/// @param fname The name of the file to remove.
void cmd_rm(VNode *parent, char *fname)
{
    // Search for the file.
    SearchResult result = binary_search_vnode_ptr_name(parent->dir.dir_count, parent->dir.children.length - 1, parent->dir.children.arr, fname, cmp_node_name);

    if (result.found)
    {
        remove_at_arr_list_vnode_ptr(&parent->dir.children, result.index);
    }
    else
    {
        printf("File of name \"%s\" does not exist.\n", fname);
    }
}

/// @brief Removes the specified directory from the system.
/// @param parent The parent of the directory to remove.
/// @param dir_name The name of the directory to remove.
void cmd_rmdir(VNode *parent, char *dir_name)
{
    // Search for the directory.
    SearchResult result = binary_search_vnode_ptr_name(0, parent->dir.dir_count - 1, parent->dir.children.arr, dir_name, cmp_node_name);

    if (result.found)
    {
        VNode *dir_node = parent->dir.children.arr[result.index];

        // If the directory is not empty, confirm if the user wants to remove all of its contents as well.
        if (dir_node->dir.children.length > 0)
        {
            char input[3];
            printf("Directory \"%s\" is not empty. Do you wish to remove all it and all of its children (y/n)?\n", dir_name);
            boolean confirm = confirm_input();

            if (confirm)
            {
                free_vnode(dir_node);
            }
            else
            {
                return;
            }
        }
        
        remove_at_arr_list_vnode_ptr(&parent->dir.children, result.index);
    }
    else
    {
        printf("File of name \"%s\" does not exist.\n", dir_name);
    }
}

void run_shell(VFS *vfs)
{
    boolean changed = FALSE;
    char input[128];

    while (TRUE)
    {
        printf("ezfs> ");
        fgets(input, sizeof(input), stdin);

        char *cmd = strtok(input, " \n");

        if (!cmd) continue;

        if (strcmp(cmd, "ls") == 0)
        {
            cmd_ls(vfs->cwd);
        }
        else if (strcmp(cmd, "cd") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_cd(vfs, arg);
        }
        else if (strcmp(cmd, "cat") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_cat(vfs->cwd, arg);
        }
        else if (strcmp(cmd, "pwd") == 0)
        {
            cmd_pwd(vfs->cwd);
            printf("\n");
        }
        else if (strcmp(cmd, "save") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_save(vfs, arg);
            changed = FALSE;
        }
        else if (strcmp(cmd, "upload") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_upload(vfs->cwd, arg);
            changed = TRUE;
        }
        else if (strcmp(cmd, "mkdir") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_mkdir(vfs->cwd, arg);
            changed = TRUE;
        }
        else if (strcmp(cmd, "rm") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_rm(vfs->cwd, arg);
            changed = TRUE;
        }
        else if (strcmp(cmd, "rmdir") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_rmdir(vfs->cwd, arg);
            changed = TRUE;
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            if (changed)
            {
                printf("Some changes to the file system appear to have be unsaved. Would you like to save them (y/n)?\n");
                boolean confirm = confirm_input();

                if (confirm)
                {
                    printf("Please enter a file to save the system to.\n");
                    fgets(input, sizeof(input), stdin);

                    char *arg = strtok(input, " \n");
                    if (arg) cmd_save(vfs, arg);
                }
            }

            break;
        }
        else
        {
            printf("Unknown command\n");
        }
    }
}

void free_vnode(VNode *node)
{
    if (!node) return;

    if (node->type == OBJ_FILE)
    {
        for (int i = 0; i < node->file.nlines; i++)
        {
            free(node->file.lines[i].arr);
        }

        free(node->file.lines);
    }
    else if (node->type == OBJ_DIRECTORY)
    {
        for (int i = 0; i < node->dir.children.length; i++)
        {
            VNode *child = node->dir.children.arr[i];
            free_vnode(child);   //recursive free
        }

        free(node->dir.children.arr);
    }

    free(node); // finally free self
}

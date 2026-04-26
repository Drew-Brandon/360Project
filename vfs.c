#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
        printf("Cannot insert the directory of the name \"%s\" into the directory named \"%s\" as that directory already exists within it\n", dir->name, parent->name);
        return TRUE;
    }

    parent->dir.dir_count++;
    insert_at_arr_list_vnode_ptr(&parent->dir.children, result.index, dir);
    return FALSE;
}

boolean insert_file_into_dir(VNode *file, VNode *parent)
{
    SearchResult result = binary_search_vnode_ptr(parent->dir.dir_count, parent->dir.children.length - 1, parent->dir.children.arr, file, cmp_node);
    
    if (result.found)
    {
        printf("Cannot insert the file of the name \"%s\" into the directory named \"%s\" as that file already exists within it\n", file->name, parent->name);
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
    if (!name)
    {
        printf("Name for directory cannot be null.\n");
        return NULL;
    }

    int i = 0;

    while (name[i])
    {
        if (name[i] == '/')
        {
            printf("Cannot have / in directory names (\"%s\")\n", name);
            return NULL;
        }
        else if (isspace(name[i]))
        {
            printf("Cannot have whitespace in directory names (\"%s\")\n", name);
            return NULL;
        }

        i++;
    }

    if (i == 0)
    {
        printf("Name for directory cannot be empty.\n");
    }

    VNode *node = malloc(sizeof(VNode));

    if (!node)
    {
        THROW_FMT("Faile to allocate memory for the directory named \"%s\"\n", name);
    }

    // Set basic info.
    strcpy(node->name, name);
    node->type = OBJ_DIRECTORY;
    node->parent = parent;
    
    if (parent && insert_dir_into_dir(node, parent))
    {
        free(node);
        return NULL;
    }
    
    // Initialize directory-specific data.
    init_arr_list_vnode_ptr(&node->dir.children);

    return node;
}

VNode *create_file(const char *name, VNode *parent)
{
    if (!name)
    {
        printf("Name for a file cannot be null.\n");
        return NULL;
    }

    int i = 0;

    while (name[i])
    {
        if (isspace(name[i]))
        {
            printf("Cannot have whitespace in file names (\"%s\")\n", name);
            return NULL;
        }

        i++;
    }

    if (i == 0)
    {
        printf("Name for file cannot be empty.\n");
    }

    VNode *node = (VNode *)malloc(sizeof(VNode));

    if (!node)
    {
        THROW_FMT("Faile to allocate memory for the file named \"%s\"\n", name);
    }

    // Set basic info.
    strncpy(node->name, name, MAX_OBJ_NAME_SIZE - 1);
    node->name[MAX_OBJ_NAME_SIZE - 1] = '\0';

    if (insert_file_into_dir(node, parent))
    {
        free(node);
        return NULL;
    }

    node->type = OBJ_FILE;
    node->parent = parent;

    // Initialize file-specific data.
    node->file.lines = NULL;
    node->file.nlines = 0;

    return node;
}

/// @brief Handles the given token from a path and uses it to navigate into another directory.
/// @param cur_node The current node being navigated from.
/// @param token The token to handle.
/// @return The node that the token navigated to.
VNode *handle_path_token(VNode *cur_node, char *token)
{
    /*
     * Handle ".." and navigate back if needed.
     * If the token is "." then don't go anywhere.
     * Otherwise, attempt to navigate to the directory the token is pointing to.
     */
    if (strcmp(token, "..") == 0)
    {
        if (cur_node->parent == NULL)
        {
            printf("Attempted to reach the parent of the directory named \"%s\", but said node does not have a parent\n", cur_node->name);
            return NULL;
        }

        cur_node = cur_node->parent;
    }
    else if (strcmp(token, ".") != 0)
    {
        SearchResult result = binary_search_vnode_ptr_name(0, cur_node->dir.dir_count - 1, cur_node->dir.children.arr, token, cmp_node_name);
        
        if (!result.found)
        {
            printf("Could not find directory named \"%s\" in the directory \"%s\"\n", token, cur_node->name);
            return NULL;
        }

        cur_node = cur_node->dir.children.arr[result.index];
    }

    return cur_node;
}

/// @brief Goes the to the directory that the series of tokens points to.
/// @param start The node to start navigating from.
/// @param tokens The tokens to navigate with.
/// @param stop The index of the token to stop navigating at.
/// @return The node that the navigation ended at.
VNode *go_to_dir(VNode *start, char **tokens, int stop)
{
    VNode *cur_node = start;
    
    // Handle each token and navigate to the appropriate directory.
    for (int i = 0; i < stop; i++)
    {
        cur_node = handle_path_token(cur_node, tokens[i]);

        if (!cur_node)
        {
            return NULL;
        }
    }

    return cur_node;
}

/// @brief Searches through the children of the specified parent using a basic regular expression.
/// @param parent The parent to search through.
/// @param start The index to start searching at.
/// @param end The index to stop searching at.
/// @param search_term The search term to search with. This is a basic regular expression that supports '*'.
/// @param node_action The function to call on each node that has a name that matches the search term.
void search_children(VNode *parent, int start, int end, char *search_term, void (*node_action)(int, VNode *))
{
    // If the search term is empty, then just accept all of the children.
    if (search_term[0] == '\0')
    {
        for (int i = start; i < end; i++)
        {
            node_action(i, parent->dir.children.arr[i]);
        }

        return;
    }

    // Search through the given directory using the given expression.
    for (int i = start; i < end; i++)
    {
        VNode *child = parent->dir.children.arr[i];

        if (star_compare(search_term, child->name))
        {
            int prev_length = parent->dir.children.length;
            node_action(i, child);

            // If the node was seemingly deleted, move the current index and end point back.
            if (prev_length != parent->dir.children.length)
            {
                i--;
                end--;
            }
        }
    }
}

/// @brief Lists out the info a given node.
/// @param index The index of the node.
/// @param node The node to list.
void list_node(int index, VNode *node)
{
    printf("%s%s\n",
        node->name,
        node->type == OBJ_DIRECTORY ? "/" : "");
}

/// @brief Lists the contents of the given directory.
/// @param start The directory to start navigating from.
/// @param path The path containing the directory to list alongside the search term.
void cmd_ls(VNode *start, char *path)
{
    // Make sure the cwd is a directory.
    if (start->type != OBJ_DIRECTORY)
    {
        printf("Not a directory\n");
        return;
    }

    // Go to the directory pointed to by the path.
    ArrayListString tokens = split_str(path, '/');
    VNode *dest = go_to_dir(start, tokens.arr, tokens.length - 1);

    if (dest)
    {
        search_children(dest, 0, dest->dir.children.length, tokens.arr[tokens.length - 1], list_node);
    }

    free_str_list(&tokens);
}

/// @brief Changes the current working directory of the system to the given path.
/// @param vfs The file system to work with.
/// @param path The path to the new working directory.
void cmd_cd(VFS *vfs, char *path)
{
    // Make sure both the file system and path are valid.
    if (!vfs || !path) return;

    // Set the starting point to the root if the first character is a /.
    VNode *start;

    if (path[0] == '/')
    {
        start = vfs->root;
    }
    else
    {
        start = vfs->cwd;
    }

    // Go to the directory the path is pointing to.
    ArrayListString tokens = split_str(path, '/');
    VNode *dest;

    // If the last token is empty, then ignore it.
    if (tokens.arr[tokens.length - 1][0] == '\0')
    {
        dest = go_to_dir(start, tokens.arr, tokens.length - 1);
    }
    else
    {
        dest = go_to_dir(start, tokens.arr, tokens.length);
    }

    if (dest)
    {
        vfs->cwd = dest;
    }

    free_str_list(&tokens);
}

/// @brief Views the contents of the specified node.
/// @param index The index of the node.
/// @param node The node to view.
void view_node(int index, VNode *node)
{
    printf("%s:\n\"", node->name);

    for (int i = 0; i < node->file.nlines - 1; i++)
    {
        printf("%s\n", node->file.lines[i].str);
    }

    printf("%s\"", node->file.lines[node->file.nlines - 1].str);
    printf("\n");
}

/// @brief Displays the contents of the files in the given path.
/// @param start The node to start navigating from.
/// @param path The path containing the directory to the files to view alongside the search term.
void cmd_cat(VNode *start, char *path)
{
    if (start->type != OBJ_DIRECTORY)
    {
        printf("Not a directory\n");
        return;
    }

    ArrayListString tokens = split_str(path, '/');
    VNode *dest = go_to_dir(start, tokens.arr, tokens.length - 1);

    if (dest)
    {
        search_children(dest, dest->dir.dir_count, dest->dir.children.length, tokens.arr[tokens.length - 1], view_node);
    }

    free_str_list(&tokens);
}

/// @brief Displays the current working directory.
/// @param node The current working directory.
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
void cmd_upload(VNode *parent, const char *path)
{
    FILE *file = fopen(path, "r");

    if (!file)
    {
        printf("Couldn't open file at location \"%s\"\n", path);
        return;
    }

    VNode *node = upload_file(file, path, parent);
    fclose(file);
}

/// @brief Offloads the file onto the local system.
/// @param index The index of the node.
/// @param node The file to offload.
void offload_file(int index, VNode *node)
{
    FILE *file = fopen(node->name, "w");

    if (!file)
    {
        printf("Failed to open file %s", node->name);
        return;
    }
    
    for (int i = 0; i < node->file.nlines - 1; i++)
    {
        fprintf(file, "%s\n", node->file.lines[i].str);
    }

    if (node->file.lines[node->file.nlines - 1].str[0] != '\0')
    {
        fprintf(file, "%s\n", node->file.lines[node->file.nlines - 1].str);
    }
    
    if (fclose(file))
    {
        printf("Failed to close file %s", node->name);
    }
}

/// @brief Offloads the given files to the local system.
/// @param start The node to start navigating from.
/// @param path The path containing the directory of the nodes to offload alongside the search term.
void cmd_offload(VNode *start, char *path)
{
    ArrayListString tokens = split_str(path, '/');
    VNode *dest = go_to_dir(start, tokens.arr, tokens.length - 1);

    if (dest)
    {
        search_children(dest, dest->dir.dir_count, dest->dir.children.length, tokens.arr[tokens.length - 1], offload_file);
    }

    free_str_list(&tokens);
}

/// @brief Creates the directory and attaches it to the specified parent.
/// @param start The node to start navigating from.
/// @param path The path containing where the directory will be created, alongside the name of the new directory.
void cmd_mkdir(VNode *start, char *path)
{
    ArrayListString tokens = split_str(path, '/');
    VNode *dest = go_to_dir(start, tokens.arr, tokens.length - 1);

    if (dest)
    {
        create_dir(tokens.arr[tokens.length - 1], dest);
    }

    free_str_list(&tokens);
}

/// @brief Removes the specified file from the system.
/// @param index The index of the file to remove.
/// @param node The file to remove.
void remove_file(int index, VNode *node)
{
    remove_at_arr_list_vnode_ptr(&node->parent->dir.children, index);
    free_vnode(node);
}

/// @brief Removes the specified files from the system.
/// @param start The node to start navigating from.
/// @param path The path containing the directory of the nodes to remove alongside the search term.
void cmd_rm(VNode *start, char *path)
{
    ArrayListString tokens = split_str(path, '/');
    VNode *dest = go_to_dir(start, tokens.arr, tokens.length - 1);

    if (dest)
    {
        search_children(dest, dest->dir.dir_count, dest->dir.children.length, tokens.arr[tokens.length - 1], remove_file);
    }

    free_str_list(&tokens);
}

/// @brief Removes the given directory.
/// @param index The index of the directory to use.
/// @param node The node to remove.
void remove_dir(int index, VNode *node)
{
    // If the directory is not empty, then confirm with the user on whether or not the children should be remove as well.
    if (node->dir.children.length > 0)
    {
        char input[3];
        printf("Directory \"%s\" is not empty. Do you wish to remove all it and all of its children (y/n)?\n", node->name);
        boolean confirm = confirm_input();

        if (!confirm)
        {
            return;
        }
    }

    remove_at_arr_list_vnode_ptr(&node->parent->dir.children, index);
    free_vnode(node);
}

/// @brief Removes the given directories from the system.
/// @param start The node to start navigating from.
/// @param path The path containing the directory to remove from alongside the search term.
void cmd_rmdir(VNode *start, char *path)
{
    ArrayListString tokens = split_str(path, '/');
    VNode *dest = go_to_dir(start, tokens.arr, tokens.length - 1);

    if (dest)
    {
        // Make sure to update the directory count if anything is deleted.
        int prev_length = dest->dir.children.length;
        search_children(dest, 0, dest->dir.dir_count, tokens.arr[tokens.length - 1], remove_dir);
        dest->dir.dir_count -= prev_length - dest->dir.children.length;
    }

    free_str_list(&tokens);
}

/// @brief Moves the given file to the specified location.
/// @param start The node to start navigating from.
/// @param from The path to the node to move.
/// @param to The path to where the node will be moved.
void cmd_mv(VNode *start, char *from, char *to)
{
    ArrayListString from_tokens = split_str(from, '/');
    VNode *from_dest = go_to_dir(start, from_tokens.arr, from_tokens.length - 1);
    ArrayListString to_tokens = split_str(to, '/');
    VNode *to_dest = go_to_dir(start, to_tokens.arr, to_tokens.length - 1);

    if (from_dest && to_dest)
    {
        // Make sure the new name is not empty.
        if (to_tokens.arr[to_tokens.length - 1][0] == '\0')
        {
            printf("File name cannot be empty\n");
        }
        else
        {
            // Find the node
            SearchResult result = binary_search_vnode_ptr_name(from_dest->dir.dir_count, from_dest->dir.children.length,
                                        from_dest->dir.children.arr, from_tokens.arr[from_tokens.length - 1], cmp_node_name);
            
            // Insert the node into its new location.
            if (result.found)
            {
                VNode *from_node = from_dest->dir.children.arr[result.index];
                remove_at_arr_list_vnode_ptr(&from_dest->dir.children, result.index);
                strncpy(from_node->name, to_tokens.arr[to_tokens.length - 1], MAX_OBJ_NAME_SIZE);
                insert_file_into_dir(from_node, to_dest);
            }
            else
            {
                printf("The file \"%s\" does not exist within the directory \"%s\"\n", from_tokens.arr[from_tokens.length - 1], from_dest->name);
            }
        }
    }

    free_str_list(&from_tokens);
    free_str_list(&to_tokens);
}

/// @brief Moves the specified directory to the given location.
/// @param start The node to start navigating from.
/// @param from The path to the directory to move.
/// @param to The path to move the directory to.
void cmd_mvdir(VNode *start, char *from, char *to)
{
    ArrayListString from_tokens = split_str(from, '/');
    VNode *from_dest = go_to_dir(start, from_tokens.arr, from_tokens.length - 1);
    ArrayListString to_tokens = split_str(to, '/');
    VNode *to_dest = go_to_dir(start, to_tokens.arr, to_tokens.length - 1);

    if (from_dest && to_dest)
    {
        // Make sure the new name is not empty.
        if (to_tokens.arr[to_tokens.length - 1][0] == '\0')
        {
            printf("Directory name cannot be empty\n");
        }
        else
        {
            // Find the directory
            SearchResult result = binary_search_vnode_ptr_name(0, from_dest->dir.dir_count,
                                        from_dest->dir.children.arr, from_tokens.arr[from_tokens.length - 1], cmp_node_name);
            
            // Move the directory
            if (result.found)
            {
                VNode *from_node = from_dest->dir.children.arr[result.index];
                remove_at_arr_list_vnode_ptr(&from_dest->dir.children, result.index);
                from_dest->dir.dir_count--;

                strncpy(from_node->name, to_tokens.arr[to_tokens.length - 1], MAX_OBJ_NAME_SIZE);
                insert_dir_into_dir(from_node, to_dest);
            }
            else
            {
                printf("The directory \"%s\" does not exist within the directory \"%s\"\n", from_tokens.arr[from_tokens.length - 1], from_dest->name);
            }
        }
    }

    free_str_list(&from_tokens);
    free_str_list(&to_tokens);
}

/// @brief Copies the given file to the specified location.
/// @param start The node to start navigating from.
/// @param from The path to the node to copy.
/// @param to The path to copy the node to.
void cmd_cp(VNode *start, char *from, char *to)
{
    ArrayListString from_tokens = split_str(from, '/');
    VNode *from_dest = go_to_dir(start, from_tokens.arr, from_tokens.length - 1);
    ArrayListString to_tokens = split_str(to, '/');
    VNode *to_dest = go_to_dir(start, to_tokens.arr, to_tokens.length - 1);

    if (from_dest && to_dest)
    {
        // Find the node.
        SearchResult result = binary_search_vnode_ptr_name(from_dest->dir.dir_count, from_dest->dir.children.length,
                                    from_dest->dir.children.arr, from_tokens.arr[from_tokens.length - 1], cmp_node_name);
        
        // Copy the node.
        if (result.found)
        {
            VNode *from_node = from_dest->dir.children.arr[result.index];
            VNode *new_node = create_file(to_tokens.arr[to_tokens.length - 1], to_dest);

            if (new_node)
            {
                new_node->file.nlines = from_node->file.nlines;
                new_node->file.lines = malloc(sizeof(Line) * new_node->file.nlines);
                
                for (int i = 0; i < new_node->file.nlines; i++)
                {
                    new_node->file.lines[i].length = from_node->file.lines[i].length;
                    new_node->file.lines[i].str = malloc(sizeof(char) * new_node->file.lines[i].length);
                    memcpy(new_node->file.lines[i].str, from_node->file.lines[i].str, new_node->file.lines[i].length * sizeof(char));
                }
            }
        }
        else
        {
            printf("The file \"%s\" does not exist within the directory \"%s\"\n", from_tokens.arr[from_tokens.length - 1], from_dest->name);
        }
    }

    free_str_list(&from_tokens);
    free_str_list(&to_tokens);
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
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_ls(vfs->cwd, arg);
            else cmd_ls(vfs->cwd, "./");
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
        else if (strcmp(cmd, "offload") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_offload(vfs->cwd, arg);
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
        else if (strcmp(cmd, "mv") == 0)
        {
            char *arg1 = strtok(NULL, " \n");
            char *arg2 = strtok(NULL, " \n");

            if (arg1 && arg2) cmd_mv(vfs->cwd, arg1, arg2);
            changed = TRUE;
        }
        else if (strcmp(cmd, "mvdir") == 0)
        {
            char *arg1 = strtok(NULL, " \n");
            char *arg2 = strtok(NULL, " \n");

            if (arg1 && arg2) cmd_mvdir(vfs->cwd, arg1, arg2);
            changed = TRUE;
        }
        else if (strcmp(cmd, "cp") == 0)
        {
            char *arg1 = strtok(NULL, " \n");
            char *arg2 = strtok(NULL, " \n");

            if (arg1 && arg2) cmd_cp(vfs->cwd, arg1, arg2);
            changed = TRUE;
        }
        else if (strcmp(cmd, "exit") == 0)
        {
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
            free(node->file.lines[i].str);
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

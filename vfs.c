#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"
#include "journaling.h"
#include "boolean.h"

DEF_ARRAY_LIST_SOURCE(VNode*, VNodePtr, vnode_ptr)

VNode *create_dir(char *name, VNode *parent)
{
    VNode *node = malloc(sizeof(VNode));
    strcpy(node->name, name);
    node->type = OBJ_DIRECTORY;
    node->parent = parent;
    init_arr_list_vnode_ptr(&node->dir.children);
    return node;
}

VNode *create_file(char *name, VNode *parent)
{
    VNode *node = (VNode *)malloc(sizeof(VNode));
    if (!node)
    {
        THROW("Failed to allocate memory for file node");
    }

    // Set basic info
    strncpy(node->name, name, MAX_FILE_NAME_SIZE - 1);
    node->name[MAX_FILE_NAME_SIZE - 1] = '\0';

    node->type = OBJ_FILE;
    node->parent = parent;

    // Initialize file-specific data
    node->file.lines = NULL;
    node->file.nlines = 0;

    return node;
}

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

void print_path(VNode *node)
{
    if (node->parent != NULL)
    {
        print_path(node->parent);
    }
    printf("/%s", node->name);
}

void cmd_save(VFS *vfs, char *save_file)
{
    FILE *file = fopen(save_file, "w");

    if (!file)
    {
        THROW_FMT("Couldn't open file at location \"%s\"", save_file);
    }

    print_dir_node(file, vfs->root, 0);
    fclose(file);
}

void run_shell(VFS *vfs)
{
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
            print_path(vfs->cwd);
            printf("\n");
        }
        else if (strcmp(cmd, "save") == 0)
        {
            char *arg = strtok(NULL, " \n");
            if (arg) cmd_save(vfs, arg);
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
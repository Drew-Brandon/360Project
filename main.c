#include <stdio.h>
#include <stdlib.h>

#include "journaling.h"
#include "file_utils.h"
#include "vfs.h"

int main(int argc, char *argv[])
{
    init_journaling();

    const char *filename = "example_files/foo.txt";

    if (argc > 1)
    {
        filename = argv[1];
    }

    FILE *in = fopen(filename, "r");
    if (!in)
    {
        perror("Failed to open input file");
        free_journaling();
        return 1;
    }

    printf("Loading EZ file: %s\n", filename);

    VNode *root = NULL;

    TRY(
    {
        root = build_ez_tree(in);

        VFS vfs;
        vfs.root = root;
        vfs.cwd = root;

        printf("EZ file loaded successfully.\n");
        printf("Entering virtual file system...\n\n");

        run_shell(&vfs);
    },
    {
        printf("Exception occurred:\n");
        print_exc_msg();
        printf("\n");

        printf("Trace:\n");
        print_exc_path();
        printf("\n");
    });

    if (root != NULL)
    {
        free_vnode(root);
    }

    fclose(in);
    free_journaling();

    printf("Program terminated.\n");
    return 0;
}
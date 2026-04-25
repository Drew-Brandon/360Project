#include <stdio.h>
#include <stdlib.h>
#include "journaling.h"
#include "file_utils.h"
#include "vfs.h"

int main(int argc, char *argv[])
{
    char *fname = "";
    FILE *in = fopen(fname, "r");

    // Open the specified input file if there is one and use it as a base for the file system.
    if (argc > 1)
    {
        fname = argv[1];
        in = fopen(fname, "r");

        if (!in)
        {
            perror("Failed to open file");
            return 1;
        }

        printf("Loading EZ file: %s\n", fname);
    }

    init_journaling();
    VNode *root = NULL;

    TRY(
    {
        /* 
         * If an input file was specified, then construct the virtual system from it.
         * Otherwise, just create a new virtual system.
         */
        if (in == NULL)
        {
            printf("Specify a name for the root directory\n");
            char root_name[MAX_OBJ_NAME_SIZE];
            fgets(root_name, sizeof(root_name), stdin);
            root = create_dir(root_name, NULL);
        }
        else
        {
            JCALL(root = build_ez_tree(in));
        }

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

    free_journaling();

    if (fclose(in))
    {
        perror("Failed to properly close file");
        return 1;
    }

    printf("Program terminated.\n");
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "journaling.h"
#include "file_utils.h"
#include "vfs.h"

FILE *open_ez_file(char *fname);

int main(int argc, char *argv[])
{
    int return_code = 0;
    char *in_name = NULL;
    FILE *in = NULL;
    VNode *root = NULL;
    init_journaling();

    TRY(
    {
        // Open the specified input file if there is one and use it as a base for the file system.
        if (argc > 1)
        {
            JCALL(in = open_ez_file(argv[1]));
        }

        /* 
         * If an input file was specified, then construct the virtual system from it.
         * Otherwise, just create a new virtual system.
         */
        if (in)
        {
            JCALL(root = build_ez_tree(in));
        }
        else
        {
            printf("Specify a name for the root directory\n");

            while (!root)
            {
                char input[MAX_OBJ_NAME_SIZE + 1];
                fgets(input, sizeof(input), stdin);
                char *root_name = strtok(input, "\n");
                root = create_dir(root_name, NULL);
            }
        }

        VFS vfs;
        vfs.root = root;
        vfs.cwd = root;

        printf("EZ file loaded successfully.\n");
        printf("Entering virtual file system...\n\n");
        run_shell(&vfs);

        if (in && fclose(in))
        {
            THROW_FMT("Failed to properly close file \"%s\"", in_name);
        }

        return_code = 0;
    },
    {
        printf("Exception occurred:\n");
        print_exc_msg();
        printf("\n");
        
        printf("Trace:\n");
        print_exc_path();
        printf("\n");

        return_code = 1;
    });

    if (root != NULL)
    {
        free_vnode(root);
    }

    free_journaling();
    printf("Program terminated.\n");
    return return_code;
}

FILE *open_ez_file(char *fname)
{
    FILE *file = fopen(fname, "r");

    if (!file)
    {
        THROW_FMT("Failed to open file \"%s\"", fname);
    }

    printf("Loading EZ file: %s\n", fname);
    return file;
}

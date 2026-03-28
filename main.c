#include <stdio.h>
#include "file_utils.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <path>\n", argv[0]);
        return 1;
    }

    handle_path(argv[1]);

    return 0;
}

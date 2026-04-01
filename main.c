#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_utils.h"
#include "file_manager.h"

 #define EZSH_MAX_INPUT 1024 

int main()
{
    char input[EZSH_MAX_INPUT];

    while (1)
    {
        printf("ezsh> "); // prompt

        if (!fgets(input, sizeof(input), stdin))
            break;

        // remove newline
        input[strcspn(input, "\n")] = 0;

        // exit command
        if (strcmp(input, "exit") == 0)
            break;

        handle_command(input);
    }

    return 0;
}






#include "file_manager.h"

void list_directory(const char *path)
{
    DIR *dir = opendir(path);
    if (!dir)
    {
        perror("opendir failed");
        return;
    }

    struct dirent *entry;

    printf("Contents of directory: %s\n", path);

    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

void display_file(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        perror("fopen failed");
        return;
    }

    char buffer[512];

    printf("Contents of file: %s\n\n", path);

    while (fgets(buffer, sizeof(buffer), file))
    {
        printf("%s", buffer);
    }

    fclose(file);
}

void handle_path(const char *path)
{
    struct stat path_stat;

    if (stat(path, &path_stat) != 0)
    {
        perror("stat failed");
        return;
    }

    if (S_ISDIR(path_stat.st_mode))
    {
        list_directory(path);
    }
    else if (S_ISREG(path_stat.st_mode))
    {
        display_file(path);
    }
    else
    {
        printf("Unknown file type.\n");
    }
}

void handle_command(char *input)
{
    char *command = strtok(input, " ");
    char *arg = strtok(NULL, " ");

    if (!command)
        return;

    // ls command
    if (strcmp(command, "ls") == 0)
    {
        if (!arg)
            arg = "."; // default to current directory

        list_directory(arg);
    }
    // cat command
    else if (strcmp(command, "cat") == 0)
    {
        if (!arg)
        {
            printf("cat: missing file argument\n");
            return;
        }

        display_file(arg);
    }
    
    else
    {
        printf("Unknown command: %s\n", command);
    }
}
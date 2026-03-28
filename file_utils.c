#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "file_utils.h"

void indent_line(FILE *ez_file, int layer)
{
    for (int i = 0; i < layer; i++)
    {
        fputc('\t', ez_file);
    }
}

void skip_indents(FILE *ez_file, int layer)
{
    for (int i = 0; i < layer; i++)
    {
        fgetc(ez_file);
    }
}

void free_ez_file(EZFile *file)
{
    for (int i = 0; i < file->nlines; i++)
    {
        free(file->lines[i]);
    }

    free(file->lines);
}

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
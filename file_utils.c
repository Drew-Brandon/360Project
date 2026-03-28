#include <stdlib.h>
#include <string.h>
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

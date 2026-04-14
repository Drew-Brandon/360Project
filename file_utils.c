#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "boolean.h"
#include "file_utils.h"
#include "journaling.h"

DEF_ARRAY_LIST_SOURCE(char, Char, ch)

DEF_ARRAY_LIST_SOURCE(ArrayListChar, String, str)

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

char peek_char(FILE *file)
{
    char ch = fgetc(file);

    if (ch != EOF)
    {
        fseek(file, -1L, SEEK_CUR);
    }
    
    return ch;
}

void skip_whitespace(FILE *file)
{
    char cur_ch;

    do
    {
        cur_ch = fgetc(file);

        if (cur_ch == EOF)
        {
            return;
        }
    } while (isspace(cur_ch));

    fseek(file, -1L, SEEK_CUR);
}

void skip_to_char(FILE *file, char ch)
{
    char cur_ch;

    do
    {
        cur_ch = fgetc(file);
    } while (cur_ch != ch && cur_ch != EOF);
}

ArrayListString scan_till_char(FILE *file, char ch, int layer)
{
    unsigned int cur_layer = layer, space_count = 0;
    char cur_ch;
    ArrayListChar *cur_line;
    ArrayListString lines;
    init_arr_list_str(&lines);
    push_null_arr_list_str(&lines);

    cur_line = &lines.arr[lines.length - 1];
    init_arr_list_ch(cur_line);

    while (TRUE)
    {
        cur_ch = fgetc(file);

        switch (cur_ch)
        {
        case EOF:
            push_arr_list_ch(cur_line, '\0');
            pack_arr_list_ch(cur_line);
            THROW_FMT("The string \"%s\" has reached the end of the file.", cur_line->arr);
        
        case '\t':
            if (cur_layer < layer)
            {
                cur_layer++;
                continue;
            }

            break;
        
        case ' ':
            if (cur_layer < layer)
            {
                space_count++;

                if (space_count == 4)
                {
                    cur_layer++;
                    space_count = 0;
                }

                continue;
            }

            break;
        
        case '\n':
            cur_layer = 0;
            push_arr_list_ch(cur_line, '\0');
            pack_arr_list_ch(cur_line);

            push_null_arr_list_str(&lines);
            cur_line = &lines.arr[lines.length - 1];
            init_arr_list_ch(cur_line);
            continue;
        }
        
        if (cur_ch == ch)
        {
            push_arr_list_ch(cur_line, '\0');
            pack_arr_list_ch(cur_line);

            pack_arr_list_str(&lines);
            return lines;
        }
        else
        {
            push_arr_list_ch(cur_line, cur_ch);
        }
    }
}

enum EZObjectType scan_header(FILE *file, int obj_name_size, char *obj_name)
{
    enum EZObjectType obj_type;
    char obj_type_str[OBJ_TYPE_SIZE];
    fscanf(file, "%4s", obj_type_str);

    if (strncmp(obj_type_str, DIR_TAG, OBJ_TYPE_SIZE) == 0)
    {
        obj_type = OBJ_DIRECTORY;
    }
    else if (strncmp(obj_type_str, FILE_TAG, OBJ_TYPE_SIZE) == 0)
    {
        obj_type = OBJ_FILE;
    }
    else
    {
        THROW_FMT("Object type \"%s\" does not exist.", obj_type);
    }

    JCALL(skip_to_char(file, ':'));
    JCALL(skip_whitespace(file));
    fscanf(file, "%256s", obj_name);
    return obj_type;
}

void free_ez_file(EZFile *file)
{
    for (int i = 0; i < file->nlines; i++)
    {
        free(file->lines[i]);
    }

    free(file->lines);
}

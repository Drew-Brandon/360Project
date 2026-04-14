#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "boolean.h"
#include "file_utils.h"
#include "journaling.h"
#include "vfs.h"

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
    char ch = getc(file);
    fseek(file, -1, SEEK_CUR);
    return ch;
}

void skip_whitespace(FILE *file)
{
    char cur_ch;

    do
    {
        cur_ch = fgetc(file);
    } while (isspace(cur_ch) && cur_ch != EOF);

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
    unsigned int cur_layer = 0;   // start at 0
    unsigned int space_count = 0;
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

        // Handle EOF
        if (cur_ch == EOF)
        {
            push_arr_list_ch(cur_line, '\0');
            pack_arr_list_ch(cur_line);

            THROW_FMT("The string \"%s\" has reached the end of the file.", cur_line->arr);
        }

        // Handle indentation BEFORE processing character
        if (cur_layer < layer)
        {
            if (cur_ch == '\t')
            {
                cur_layer++;
                continue;
            }
            else if (cur_ch == ' ')
            {
                space_count++;

                if (space_count == 4)
                {
                    cur_layer++;
                    space_count = 0;
                }
                continue;
            }
            else
            {
                // Hit non-indent char → stop indent tracking
                cur_layer = layer;
            }
        }

        // Newline handling
        if (cur_ch == '\n')
        {
            cur_layer = 0;
            space_count = 0;

            push_arr_list_ch(cur_line, '\0');
            pack_arr_list_ch(cur_line);

            push_null_arr_list_str(&lines);
            cur_line = &lines.arr[lines.length - 1];
            init_arr_list_ch(cur_line);

            continue;
        }

        // End condition
        if (cur_ch == ch)
        {
            push_arr_list_ch(cur_line, '\0');
            pack_arr_list_ch(cur_line);

            pack_arr_list_str(&lines);
            return lines;
        }

        // Normal character
        push_arr_list_ch(cur_line, cur_ch);
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

VNode *build_ez_tree(FILE *in)
{
    char name[MAX_FILE_NAME_SIZE];

    JCALL(enum EZObjectType type = scan_header(in, MAX_FILE_NAME_SIZE, name));

    if (type != OBJ_DIRECTORY)
    {
        THROW("Root must be a directory");
    }

    return scan_dir_node(in, name, NULL);
}

VNode *scan_dir_node(FILE *in, char *dname, VNode *parent)
{
    VNode *dir = create_dir(dname, parent);

    // Move to '{'
    JCALL(skip_to_char(in, '{'));
    fgetc(in); // consume '{'

    char obj_name[MAX_FILE_NAME_SIZE];

    while (TRUE)
    {
        skip_whitespace(in);

        char next = peek_char(in);

        // End of directory
        if (next == '}' || next == EOF)
        {
            fgetc(in); // consume '}'
            break;
        }

        // Read object header
        JCALL(enum EZObjectType obj_type =
            scan_header(in, MAX_FILE_NAME_SIZE, obj_name));

        VNode *child = NULL;

        switch (obj_type)
        {
        case OBJ_DIRECTORY:
            child = scan_dir_node(in, obj_name, dir);
            break;

        case OBJ_FILE:
            child = scan_file_node(in, obj_name, dir);
            break;

        default:
            THROW("Unknown object type");
        }

        // Push child into directory
        push_arr_list_vnode_ptr(&dir->dir.children, child);

    }

    return dir;
}

VNode *scan_file_node(FILE *in, char *fname, VNode *parent)
{
    VNode *file = create_file(fname, parent);

    // Move to opening quote
    skip_to_char(in, '\"');  // already consumes the quote

    JCALL(ArrayListString lines =
        scan_till_char(in, '\"', 0));

    file->file.lines = lines.arr;
    file->file.nlines = lines.length;

    return file;
}
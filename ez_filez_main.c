#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boolean.h"
#include "file_utils.h"
#include "journaling.h"
#define MAX_STR_LENGTH 128
#define MAX_FILE_SIZE 1024

void scan_ez_file(FILE *in, FILE *out, int layer);
void scan_dir(FILE *in, FILE *out, char *dname, int layer);
void scan_file(FILE *in, FILE *out, char *fname, int layer);

int main(int argc, char *argv[])
{
    init_journaling();
    FILE *foo = fopen("example_files/foo.txt", "rb+");
    FILE *foo_new = fopen("example_files/foo_new.txt", "w");
    printf("Reading new file...\n");

    TRY(
        JCALL(scan_ez_file(foo, foo_new, 0));
    ,
        print_exc_msg();
        printf("\n");
        print_exc_path();
        printf("\n");
    );

    fclose(foo);
    fclose(foo_new);
    free_journaling();
    return 0;
}

void scan_ez_file(FILE *in, FILE *out, int layer)
{
    char obj_name[MAX_FILE_NAME_SIZE];

    while (TRUE)
    {
        skip_whitespace(in);
        
        if (peek_char(in) == '}' || peek_char(in) == EOF)
        {
            fgetc(in);
            break;
        }

        skip_whitespace(in);
        JCALL(enum EZObjectType obj_type = scan_header(in, MAX_FILE_NAME_SIZE, obj_name));

        switch (obj_type)
        {
            case OBJ_DIRECTORY:
                printf("Scanning directory: %s\n", obj_name);
                JCALL(scan_dir(in, out, obj_name, layer));
                break;
            case OBJ_FILE:
                printf("Scanning file: %s\n", obj_name);
                JCALL(scan_file(in, out, obj_name, layer));
                break;
        }
    }
}

void scan_dir(FILE *in, FILE *out, char *dname, int layer)
{
    indent_line(out, layer);
    fprintf(out, "diry:%s\n", dname);
    skip_to_char(in, '{');
    fgetc(in);

    indent_line(out, layer);
    fputs("{\n", out);
    
    JCALL(scan_ez_file(in, out, layer + 1));

    indent_line(out, layer);
    fputs("}\n", out);
}

void scan_file(FILE *in, FILE *out, char *fname, int layer)
{
    char file_str[MAX_FILE_SIZE];

    indent_line(out, layer);
    fprintf(out, "file:%s\n", fname);
    skip_to_char(in, '\"');
    JCALL(ArrayListString lines = scan_till_char(in, '\"', layer + 1));
    fgetc(in);

    indent_line(out, layer + 1);
    fprintf(out, "\"%s", lines.arr[0].arr);
    
    for (int i = 1; i < lines.length; i++)
    {
        fputc('\n', out);
        indent_line(out, layer + 1);
        fputs(lines.arr[i].arr, out);
        free(lines.arr[i].arr);
    }
    
    free(lines.arr);
    fputs("\"\n", out);
}

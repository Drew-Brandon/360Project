#include <stdio.h>
#include "journaling.h"

DEF_ARRAY_LIST_SOURCE(LineDest, LineDest, line_dest)
DEF_ARRAY_LIST_SOURCE(JData, JData, j_data)

static ArrayListJData j_data_list;

void _push_line_dest(int line, char *file)
{
    LineDest ld;
    ld.num = line;
    ld.fpath = file;
    push_arr_list_line_dest(&j_data_list.arr[j_data_list.length - 1].ld_list, ld);
}

void _pop_line_dest()
{
    free(j_data_list.arr[j_data_list.length - 1].ld_list.arr);
    pop_arr_list_j_data(&j_data_list);
}

void _push_j_data()
{
    push_null_arr_list_j_data(&j_data_list);
    init_arr_list_line_dest(&j_data_list.arr[j_data_list.length - 1].ld_list);
}

jmp_buf *_jmp_point()
{
    return &j_data_list.arr[j_data_list.length - 1].buf;
}

void _throw(int exc_code)
{
    longjmp(j_data_list.arr[j_data_list.length - 1].buf, exc_code);
}

void init_journaling()
{
    init_arr_list_j_data(&j_data_list);
}

void free_journaling()
{
    free(j_data_list.arr);
}

void print_line_dest(LineDest dest, int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf(" ");
    }

    printf("%s:%d\n", dest.fpath, dest.num);
}

void print_exc_path()
{
    ArrayListLineDest lines = j_data_list.arr[j_data_list.length - 1].ld_list;

    for (int i = lines.length - 1; i >= 0; i--)
    {
        print_line_dest(lines.arr[i], lines.length - 1 - i);
    }
}

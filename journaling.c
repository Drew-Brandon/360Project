#include <stdio.h>
#include <stdarg.h>
#include "journaling.h"

DEF_ARRAY_LIST_SOURCE(LineDest, LineDest, line_dest)
DEF_ARRAY_LIST_SOURCE(JData, JData, j_data)

static char exc_msg[MAX_EXC_MSG_SIZE];
static ArrayListJData j_data_list;

char *_get_exc_msg()
{
    return exc_msg;
}

void _push_line_dest(const int line, char *file)
{
    if (j_data_list.length == 0)
    {
        return;
    }

    LineDest ld;
    ld.num = line;
    ld.fpath = file;
    push_arr_list_line_dest(&j_data_list.arr[j_data_list.length - 1].ld_list, ld);
}

void _pop_line_dest(void)
{
    if (j_data_list.length == 0)
    {
        return;
    }

    pop_arr_list_line_dest(&j_data_list.arr[j_data_list.length - 1].ld_list);
}

void _pop_j_dest(void)
{
    free(j_data_list.arr[j_data_list.length - 1].ld_list.arr);
    pop_arr_list_j_data(&j_data_list);
}

void _push_j_data(void)
{
    push_null_arr_list_j_data(&j_data_list);
    init_arr_list_line_dest(&j_data_list.arr[j_data_list.length - 1].ld_list);
}

jmp_buf *_jmp_point(void)
{
    return &j_data_list.arr[j_data_list.length - 1].buf;
}

void _throw(const char *new_exc_msg)
{
    strncpy(exc_msg, new_exc_msg, MAX_EXC_MSG_SIZE);
    longjmp(j_data_list.arr[j_data_list.length - 1].buf, 1);
}

void _throw_null()
{
    longjmp(j_data_list.arr[j_data_list.length - 1].buf, 1);
}

/*void _throw_fmt(const char *new_exc_msg, ...)
{
    va_list args;

    va_start(args, new_exc_msg);
    snprintf(exc_msg, MAX_EXC_MSG_SIZE - 1, new_exc_msg, args);
    va_end(args);

    longjmp(j_data_list.arr[j_data_list.length - 1].buf, 1);
}*/

void init_journaling(void)
{
    init_arr_list_j_data(&j_data_list);
}

void free_journaling(void)
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

void print_exc_msg()
{
    printf("%s", exc_msg);
}

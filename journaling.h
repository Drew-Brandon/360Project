#ifndef _JOURNALING_H_
#define _JOURNALING_H_
#include <setjmp.h>
#include "array_list.h"

typedef struct
{
    int num;
    char *fpath;
} LineDest;

DEF_ARRAY_LIST_HEADER(LineDest, LineDest, line_dest)

typedef struct
{
    jmp_buf buf;
    ArrayListLineDest ld_list;
} JData;

DEF_ARRAY_LIST_HEADER(JData, JData, j_data)

void _push_line_dest(int line, char *file);
void _pop_line_dest();
void _push_j_data();
jmp_buf *_jmp_point();
void _throw(int exc_code);
void init_journaling();
void free_journaling();
void print_exc_path();

#define THROW(exc_code) \
_push_line_dest(__LINE__, __FILE__); \
_throw(exc_code); \

#define JCALL(func) \
_push_line_dest(__LINE__, __FILE__); \
func;

#define TRY(content) \
_push_j_data(); \
\
if (setjmp(*_jmp_point()) == 0) \
content \
_pop_line_dest();

#endif

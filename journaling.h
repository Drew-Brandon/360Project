#ifndef _JOURNALING_H_
#define _JOURNALING_H_
#include <setjmp.h>
#include "array_list.h"
#define MAX_EXC_MSG_SIZE 256

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

/// @brief Pushes the specified line destination to the current journal's list.
/// @param line The line number of the destination.
/// @param file The file of the destination.
void _push_line_dest(const int line, char *file);

/// @brief Pops the line destination off the current journal's list.
void _pop_line_dest(void);

/// @brief Pops the journaling data off the current journal list.
void _pop_j_dest(void);

/// @brief Pushes a new list of journaling data to the current journal list.
void _push_j_data(void);

/// @brief Returns a pointer to the jump buffer of the current journal.
/// @return The pointer to the jump buffer.
jmp_buf *_jmp_point(void);

/// @brief Throws an exception with the specified message.
/// @param new_exc_msg The message to use for the exception.
void _throw(const char *new_exc_msg);

/// @brief Throws an exception with the specified formatted message.
/// @param new_exc_msg The message/format to use for the exception.
/// @param ... The variables to insert into the message format.
void _throw_fmt(const char *new_exc_msg, ...);

/// @brief Initializes the journaling system for the program.
void init_journaling(void);

/// @brief Frees up the journaling system for the program.
void free_journaling(void);

/// @brief Prints the path to the exception that was thrown.
void print_exc_path(void);

/// @brief Prints the message that the exception was thrown with.
void print_exc_msg(void);

/// @brief A macro meant to throw an exception with a message.
/// This message can be later displayed via the print_exc_msg() function.
/// @param exc_msg The exception message to throw.
#define THROW(exc_msg) \
_push_line_dest(__LINE__, __FILE__); \
_throw(exc_msg) \

/// @brief A macro meant to throw an exception with a formatted message.
/// This message can be later displayed via the print_exc_msg() function.
/// This acts similarly to functions like printf where a format is first specified,
/// which is then followed by the input variables to be placed into said format.
/// @param exc_msg The formatted exception message to throw.
/// @param ... The input variables to place into the formatted message.
#define THROW_FMT(exc_msg, ...) \
_push_line_dest(__LINE__, __FILE__); \
_throw_fmt(exc_msg, __VA_ARGS__) \

/// @brief A macro meant to journal a function call or code execution by storing the line and file it was stored on.
/// This location can be later displayed via the print_exc_path() function.
/// @param func The function to call or code to execute.
#define JCALL(func) \
_push_line_dest(__LINE__, __FILE__); \
func; \
_pop_line_dest()

/// @brief A macro meant to act as a point in which the program will jump back to if any errors occur.
/// @param try_content The code/content to that may contain or link up to an exception that will trigger a jump back.
/// @param catch_content The code/content that will be triggered when an exception is thrown.
#define TRY(try_content, catch_content) \
_push_j_data(); \
\
if (setjmp(*_jmp_point()) == 0) \
{ \
    try_content \
} \
else \
{ \
    catch_content \
} \
\
_pop_j_dest()

#endif

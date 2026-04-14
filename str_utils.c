#include "boolean.h"
#include "str_utils.h"
#include "journaling.h"

DEF_ARRAY_LIST_SOURCE(char, Char, ch)

DEF_ARRAY_LIST_SOURCE(ArrayListChar, String, str)

ArrayListString split_str(const char *str, const char split_ch)
{
    int i = 0;
    char ch = str[0];

    ArrayListString splices;
    init_arr_list_str(&splices);

    push_null_arr_list_str(&splices);
    ArrayListChar *cur_splice = &splices.arr[splices.length - 1];
    init_arr_list_ch(cur_splice);

    while (ch)
    {
        if (ch == split_ch)
        {
            push_arr_list_ch(cur_splice, '\0');
            pack_arr_list_ch(cur_splice);

            i++;
            ch = str[i];

            if (ch)
            {
                push_null_arr_list_str(&splices);
                cur_splice = &splices.arr[splices.length - 1];
                init_arr_list_ch(cur_splice);
            }
        }
        else
        {
            push_arr_list_ch(cur_splice, ch);

            i++;
            ch = str[i];
        }
    }

    push_arr_list_ch(cur_splice, '\0');
    pack_arr_list_ch(cur_splice);
    pack_arr_list_str(&splices);

    return splices;
}

#include <stdio.h>
#include "str_utils.h"

int main(void)
{
    ArrayListString splices = split_str("foo/bar/twist", '/');

    for (int i = 0; i < splices.length; i++)
    {
        printf("%s\n", splices.arr[i].arr);
        free(splices.arr[i].arr);
    }

    free(splices.arr);
}

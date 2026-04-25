#include <stdio.h>
#include <string.h>
#include "array_list.h"

DEF_BINARY_SEARCH_SOURCE(char *, char *, str)

int main(void)
{
    char *strs[] = {"subdir", "Bar.txt", "Foo.txt"};
    int strs_n = sizeof(strs) / sizeof(char *);
    SearchResult result = binary_search_str(strs_n, strs, "subdir", strcmp);
    printf("%u %d\n", result.found, result.index);

    return 0;
}

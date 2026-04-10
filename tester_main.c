#include <stdio.h>

int main(void)
{
    const int STR_SIZE = 8;
    char str[STR_SIZE];

    scanf("%*s", STR_SIZE, str);
    printf("%s\n", str);
}

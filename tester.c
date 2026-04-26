#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void print(char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    printf("%s\n", va_arg(args, char*));
    va_end(args);
}

int main(void)
{
    print("Foo%s\n", "bar");
    
    return 0;
}

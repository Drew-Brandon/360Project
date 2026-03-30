#include <stdio.h>
#include "journaling.h"

void bar()
{
    printf("bar\n");
    THROW(1)
}

void foo()
{
    printf("foo\n");

    TRY(
    {
        JCALL(bar())
    }
    else
    {
        printf("Error at:\n");
        print_exc_path();
    })

    THROW(1)
}

int main(void)
{
    init_journaling();

    TRY(
    {
        JCALL(foo())
    }
    else
    {
        printf("Error at:\n");
        print_exc_path();
    })

    printf("Stop Program...\n");
    free_journaling();
    return 0;
}

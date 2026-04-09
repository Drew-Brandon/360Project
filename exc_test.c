#include <stdio.h>
#include "journaling.h"

void bar()
{
    printf("bar\n");
    THROW("Test Exception")
}

void foo()
{
    printf("foo\n");

    TRY(
        JCALL(bar())
    ,
        print_exc_msg();
        printf(":\n");
        print_exc_path();
    )

    THROW("2nd Test Exception")
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
        print_exc_msg();
        printf(":\n");
        print_exc_path();
    })

    printf("Stop Program...\n");
    free_journaling();
    return 0;
}

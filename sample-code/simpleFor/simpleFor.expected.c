#include <stdio.h>

int main()
{
    int c = 0;
    for(int i = 0; i < 10; ++i)
    {
fprintf(stderr, "branch 1\n");
        c += i;
    }
fprintf(stderr, "branch 2\n");
    return 0;
}
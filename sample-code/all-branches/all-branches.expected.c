/**
 * shows an example of every branch that the branch-track program can instrument
 * see all-branches.expected.c for the expected output
 * see all-branches-dict.txt for the dictionary
*/

#include <stdio.h>

int main(int argc, char** argv)
{
    if(argc < 2)
    {
fprintf(stderr, "branch 1\n");

        printf("no arguments passed\n");
fprintf(stderr, "function %p\n", &printf);
        return 1;
    }
    else
    {
fprintf(stderr, "branch 2\n");

        printf("arguments were passed\n");
fprintf(stderr, "function %p\n", &printf);
    }

    int x = 0;
    do
    {
fprintf(stderr, "branch 3\n");
        ++x;
        printf("do x: %d\n", x);
fprintf(stderr, "function %p\n", &printf);
        if(x > 5)
        {
fprintf(stderr, "branch 9\n");

            fprintf(stderr, "branch 10\n");
break;
        }
    } while(1);
fprintf(stderr, "branch 4\n");

    x = -1;

    while(x < 4)
    {
fprintf(stderr, "branch 5\n");
        ++x;
        if(x == 1)
        {
fprintf(stderr, "branch 11\n");

            fprintf(stderr, "branch 12\n");
continue;
        }
        printf("do x: %d\n", x);
fprintf(stderr, "function %p\n", &printf);
    }
fprintf(stderr, "branch 6\n");

    for(int i = 4; i > 0; --i)
    {
fprintf(stderr, "branch 7\n");
        printf("for i: %d\n", i);
fprintf(stderr, "function %p\n", &printf);
    }
fprintf(stderr, "branch 8\n");
}
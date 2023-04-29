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
        printf("no arguments passed\n");
        return 1;
    }
    else
    {
        printf("arguments were passed\n");
    }

    int x = 0;
    do
    {
        ++x;
        printf("do x: %d\n", x);
        if(x > 5)
        {
            break;
        }
    } while(1);

    x = -1;

    while(x < 4)
    {
        ++x;
        if(x == 1)
        {
            continue;
        }
        printf("while x: %d\n", x);
    }

    for(int i = 4; i > 0; --i)
    {
        printf("for i: %d\n", i);
    }
}
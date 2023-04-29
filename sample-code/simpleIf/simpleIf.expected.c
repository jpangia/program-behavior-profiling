/**
 * Takes a number from commandline.
 * prints "even" or "odd" based on the number
 * 
 * usage: ./simple_if <number>
*/
#include <stdio.h>


int main(int argc, char** argv)
{   
    //check the commandline args
    if(argc != 2)
    {
fprintf(stderr, "branch 1\n");

        printf("usage: ./simple_if <number>\n");
fprintf(stderr, "function %p\n", &printf);
        return 1;
    }

    int num = 0;

    sscanf(argv[1], "%d", &num);
fprintf(stderr, "function %p\n", &sscanf);

    if(num % 2 == 0)
    {
fprintf(stderr, "branch 2\n");

        printf("%d is even\n", num);
fprintf(stderr, "function %p\n", &printf);
    }
    else
    {
fprintf(stderr, "branch 3\n");

        printf("%d is odd\n", num);
fprintf(stderr, "function %p\n", &printf);
    }


}
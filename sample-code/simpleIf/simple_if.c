/**
 * Takes a number from commandline.
 * prints "even" or "odd" based on the number
 * 
 * usage: ./simple_if <number>
*/
#include <stdio.h>


int main(int argc, char** argv)
{   
    do
    {
        printf("oink\n");
        continue;
    } while (0);
    
    //check the commandline args
    if(argc != 2)
    {
        printf("usage: ./simple_if <number>\n");
        return 1;
    }

    int num = 0;

    sscanf(argv[1], "%d", &num);

    if(num % 2 == 0)
    {
        printf("%d is even\n", num);
    }
    else
    {
        printf("%d is odd\n", num);
    }


}
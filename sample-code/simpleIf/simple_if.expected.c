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
    if(argc != 2) //br
    {
        printf("usage: ./simple_if <number>\n"); //br?
        return 1; //br?
    }

    int num = 0;

    sscanf(argv[1], "%d", &num); //br?

    if(num % 2 == 0) //br
    {
        printf("%d is even\n", num); //br?
    }
    else //br
    {
        printf("%d is odd\n", num); //br?
    }


}
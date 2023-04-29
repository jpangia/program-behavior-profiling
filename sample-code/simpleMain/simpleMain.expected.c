/**
 * compile with: gcc -o simple simple.c 
 * 
*/
#include <stdio.h>
int main()
{
    printf("hello world\n");
fprintf(stderr, "function %p\n", &printf);
    return 0;
}
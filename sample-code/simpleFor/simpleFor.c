#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main()
{
    int x = atoi("1");
    x = atoi("1");
    for(int i = 1; i < 10; ++i)
    {
        printf("i: %d\n", i);
        do
        {
            if(false)
            {
                continue;
            }
            else
            {
                break;
            }
        } while(false);

        switch(i)
        {
            case 5:
            break;
            case 6:
            break;
            case 7:
            break;
            default:
            break;
        }
    }

    return 0;
}
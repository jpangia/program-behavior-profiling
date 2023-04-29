/**
 * Multiplies two 100x100 integer matrices together
 * pass the dimension as a single integer to command line.
 * 
 * defaults to 10 if no dimension passed
 * 
 * taken in part from: https://www.geeksforgeeks.org/c-program-to-multiply-two-matrices/
*/

#include <stdio.h>
#include <stdlib.h>

#define N 100

int main()
{
    int mat1[N][N];
    int mat2[N][N];
    int res[N][N];
    //populate the matrices
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            mat1[i][j] = i+1;
            mat2[i][j] = j+1;
            res[i][j] = 0;
        }
    }

    //multiply the matrices (taken from link)
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            for (int k = 0; k < N; k++)
            {
                res[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    printf("result:\n");
    for(int i = 0 ; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            printf("%d ", res[i][j]);
        }
        printf("\n");
    }
}
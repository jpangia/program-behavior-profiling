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
fprintf(stderr, "branch 1\n");
        for(int j = 0; j < N; ++j)
        {
fprintf(stderr, "branch 7\n");
            mat1[i][j] = i+1;
            mat2[i][j] = j+1;
            res[i][j] = 0;
        }
fprintf(stderr, "branch 8\n");
    }
fprintf(stderr, "branch 2\n");

    //multiply the matrices (taken from link)
    for (int i = 0; i < N; i++) 
    {
fprintf(stderr, "branch 3\n");
        for (int j = 0; j < N; j++) 
        {
fprintf(stderr, "branch 9\n");
            for (int k = 0; k < N; k++)
            {
fprintf(stderr, "branch 11\n");
                res[i][j] += mat1[i][k] * mat2[k][j];
            }
fprintf(stderr, "branch 12\n");
        }
fprintf(stderr, "branch 10\n");
    }
fprintf(stderr, "branch 4\n");

    printf("result:\n");
fprintf(stderr, "function %p\n", &printf);
    for(int i = 0 ; i < N; ++i)
    {
fprintf(stderr, "branch 5\n");
        for(int j = 0; j < N; ++j)
        {
fprintf(stderr, "branch 13\n");
            printf("%d ", res[i][j]);
fprintf(stderr, "function %p\n", &printf);
        }
fprintf(stderr, "branch 14\n");
        printf("\n");
fprintf(stderr, "function %p\n", &printf);
    }
fprintf(stderr, "branch 6\n");
}
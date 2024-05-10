#include <stdio.h>
#include <stdlib.h>


double **read_matrix(const char *filename, int dim)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error opening file %s for reading.\n", filename);
        return NULL;
    }

    double **matrix = (double **)malloc(dim * sizeof(double *));
    for (int i = 0; i < dim; i++)
    {
        matrix[i] = (double *)malloc(dim * sizeof(double));
        for (int j = 0; j < dim; j++)
        {
            fscanf(file, "%lf", &matrix[i][j]);
        }
    }

    fclose(file);
    return matrix;
}



double **multiply_matrices(double **A, double **B, int dim)
{
    double **C = (double **)malloc(dim * sizeof(double *));
    for (int i = 0; i < dim; i++)
    {
        C[i] = (double *)calloc(dim, sizeof(double));
        for (int j = 0; j < dim; j++)
        {
            for (int k = 0; k < dim; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}


void write_matrix(const char *filename, double **matrix, int dim)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error opening file %s for writing.\n", filename);
        return;
    }

    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            fprintf(file, "%.4f ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please give the dimension of matrix\n");
        return 0;
    }

    int dim = atoi(argv[1]);
    if (dim <= 0)
    {
        printf("Invalid dimension size.\n");
        return 0;
    }

    double **A = read_matrix("matrix_A.out", dim);
    double **B = read_matrix("matrix_B.out", dim);
    if (A == NULL || B == NULL)
    {
        return 1; 
    }

    double **C = multiply_matrices(A, B, dim);
    write_matrix("matrix_C.out", C, dim);


    free(A);
    free(B);
    free(C);

    printf("Matrix multiplication complete. Result saved in matrix_C.out.\n");

    return 0;
}

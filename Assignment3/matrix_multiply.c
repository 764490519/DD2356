#include <stdio.h>
#include <stdlib.h>

// Function to read a matrix from a file
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

// Function to multiply two matrices
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

// Function to write a matrix to a file
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
            fprintf(file, "%.2f ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <dimension>\n", argv[0]);
        return 1;
    }

    int dim = atoi(argv[1]);
    if (dim <= 0)
    {
        fprintf(stderr, "Dimension must be a positive integer.\n");
        return 1;
    }

    double **A = read_matrix("matrix_A.out", dim);
    double **B = read_matrix("matrix_B.out", dim);
    if (A == NULL || B == NULL)
    {
        return 1; // Error handling in read_matrix will explain the failure
    }

    double **C = multiply_matrices(A, B, dim);
    write_matrix("matrix_C.out", C, dim);

    // Clean up memory
    for (int i = 0; i < dim; i++)
    {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    printf("Matrix multiplication complete. Result saved in matrix_C.out.\n");

    return 0;
}

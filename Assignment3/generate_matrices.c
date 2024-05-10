#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate a matrix and write to a file
void generate_and_write_matrix(const char *filename, int dim)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Failed to open file %s for writing.\n", filename);
        return;
    }

    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            // Random double between 0 and 100
            double value = (double)rand() / RAND_MAX * 100.0;
            fprintf(file, "%f ", value);
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

    srand(time(NULL));

    // Generate matrix A
    generate_and_write_matrix("matrix_A.out", dim);

    // Generate matrix B
    generate_and_write_matrix("matrix_B.out", dim);

    printf("Matrices of dimension %dx%d were generated and written to matrix_A.out and matrix_B.out.\n", dim, dim);

    return 0;
}
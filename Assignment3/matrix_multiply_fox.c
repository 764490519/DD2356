#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function to read a block of the matrix from file
void read_matrix_block(const char *filename, double *block, int block_size, int grid_size, int block_row, int block_col)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error opening file %s for reading.\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int full_size = block_size * grid_size;
    double *temp = malloc(full_size * full_size * sizeof(double));

    for (int i = 0; i < full_size; i++)
    {
        for (int j = 0; j < full_size; j++)
        {
            fscanf(file, "%lf", &temp[i * full_size + j]);
        }
    }

    for (int i = 0; i < block_size; i++)
    {
        for (int j = 0; j < block_size; j++)
        {
            block[i * block_size + j] = temp[(block_row * block_size + i) * full_size + block_col * block_size + j];
        }
    }

    free(temp);
    fclose(file);
}

// Function to perform local matrix multiplication
void local_matrix_multiply(double *A, double *B, double *C, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            double sum = 0.0;
            for (int k = 0; k < size; k++)
            {
                sum += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] += sum;
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = sqrt(size);
    if (n * n != size)
    {
        fprintf(stderr, "The number of processes must be a perfect square.\n");
        MPI_Finalize();
        return 1;
    }

    int block_size = atoi(argv[1]); // Assume the matrix size is evenly divisible by n
    double *A = malloc(block_size * block_size * sizeof(double));
    double *B = malloc(block_size * block_size * sizeof(double));
    double *C = calloc(block_size * block_size, sizeof(double));

    int row = rank / n;
    int col = rank % n;

    read_matrix_block("matrix_A.out", A, block_size, n, row, col);
    read_matrix_block("matrix_B.out", B, block_size, n, row, col);

    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, row, rank, &row_comm);

    for (int k = 0; k < n; k++)
    {
        int root = (col + k) % n;
        MPI_Bcast(B, block_size * block_size, MPI_DOUBLE, root, row_comm);
        local_matrix_multiply(A, B, C, block_size);

        int next = (rank + n) % size;
        int prev = (rank - n + size) % size;
        MPI_Sendrecv_replace(B, block_size * block_size, MPI_DOUBLE, next, 0, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Comm_free(&row_comm);

    double *final_C = NULL;
    if (rank == 0)
    {
        final_C = malloc(size * block_size * block_size * sizeof(double));
    }
    MPI_Gather(C, block_size * block_size, MPI_DOUBLE, final_C, block_size * block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        FILE *file = fopen("matrix_C.out", "w");
        for (int i = 0; i < n * block_size; i++)
        {
            for (int j = 0; j < n * block_size; j++)
            {
                fprintf(file, "%.2f ", final_C[i * n * block_size + j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
        free(final_C);
    }

    free(A);
    free(B);
    free(C);
    MPI_Finalize();
    return 0;
}
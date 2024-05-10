#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Read a block of the matrix from file
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
            // block[i * block_size + j] = temp[(block_row * block_size + i) * full_size + block_col * block_size + j];
            block[i * block_size + j] = temp[(block_row * block_size + i) * full_size + (block_col * block_size + j)];
        }
    }

    free(temp);
    fclose(file);
}

int getidx(int row, int col, int sqr)
{
    return ((row + sqr) % sqr) * sqr + (col + sqr) % sqr;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); // number of processor

    int grid_size = sqrt(size); // processor matrix size
    if (grid_size * grid_size != size)
    {
        fprintf(stderr, "The number of processes must be a perfect square.\n");
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);                // matrix size
    int blockSize = n / grid_size;              // size of block
    int blockNum = blockSize * blockSize; // count of element in each block

    double *A = malloc(n * n * sizeof(double));
    double *B = malloc(n * n * sizeof(double));
    double *C = calloc(n * n, sizeof(double));
    double *tempC = malloc(n * n * sizeof(double));

    double *blockA = malloc(blockNum * sizeof(double));
    double *blockB = malloc(blockNum * sizeof(double));
    double *blockC = malloc(blockNum * sizeof(double));
    double *tmpA = malloc(blockNum * sizeof(double));
    double *tmpB = malloc(blockNum * sizeof(double));

    int row = rank / grid_size; // position of processor in matrix
    int col = rank % grid_size;

    read_matrix_block("matrix_A.out", blockA, blockSize, grid_size, row, col);
    read_matrix_block("matrix_B.out", blockB, blockSize, grid_size, row, col);

    if (rank == 3)
    {

        printf("col = %d\n", col);
        printf("row = %d\n", row);

        printf("The matrix A is:\n");
        for (int i = 0; i < blockSize; i++)
        {
            for (int j = 0; j < blockSize; j++)
            {
                printf("%f ", blockA[i * blockSize + j]);
            }
            printf("\n");
        }

        printf("The matrix B is:\n");
        for (int i = 0; i < blockSize; i++)
        {
            for (int j = 0; j < blockSize; j++)
            {
                printf("%f ", blockB[i * blockSize + j]);
            }
            printf("\n");
        }
    }

    int send_col = row;
    int idxmin, idxmax;

    for (int steps = 0; steps < grid_size; steps++)
    {
        // send blockA to other processor in the same line
        if (col == send_col)
        {
            idxmin = getidx(row, 0, grid_size);
            idxmax = getidx(row, grid_size - 1, grid_size);
            for (int i = idxmin; i <= idxmax; i++)
            {
                if (i == rank)
                {
                    continue;
                }
                // send blockA to other processor in the same line
                MPI_Send(blockA, blockNum, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            }
            memcpy(tmpA, blockA, blockNum * sizeof(double));
        }
        else
        {
            MPI_Recv(tmpA, blockNum, MPI_DOUBLE, getidx(row, send_col, grid_size), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        send_col = (send_col + 1) % grid_size;

        // calculate
        for (int i = 0; i < blockSize; i++)
        {
            for (int j = 0; j < blockSize; j++)
            {
                double sum = blockC[i * blockSize + j];
                for (int k = 0; k < blockSize; k++)
                    sum += tmpA[i * blockSize + k] * blockB[k * blockSize + j];
                blockC[i * blockSize + j] = sum;
            }
        }

        // update
        MPI_Status status;
        MPI_Sendrecv(blockB, blockNum, MPI_DOUBLE, getidx(row - 1, col, grid_size), 2, tmpB, blockNum, MPI_DOUBLE, getidx(row + 1, col, grid_size), 2, MPI_COMM_WORLD, &status);
        memcpy(blockB, tmpB, blockNum * sizeof(double));
    }

    // Gathering the matrix C
    MPI_Gather(blockC, blockNum, MPI_DOUBLE, tempC, blockNum, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int proc = 0; proc < size; proc++)
        {
            int grid_row = proc / grid_size;
            int grid_col = proc % grid_size;
            for (int i = 0; i < blockSize; i++)
            {
                for (int j = 0; j < blockSize; j++)
                {
                    int global_row = grid_row * blockSize + i;
                    int global_col = grid_col * blockSize + j;

                    int tempC_index = proc * blockNum + i * blockSize + j;

                    C[global_row * n + global_col] = tempC[tempC_index];
                }
            }
        }
    }

    if (rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                printf("%.2f ", C[i * n + j]);
            }
            printf("\n");
        }
    }

    if (rank == 0)
    {
        FILE *file = fopen("matrix_C_FOX.out", "w");
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                fprintf(file, "%f ", C[i * n + j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }

    free(A);
    free(B);
    free(C);
    MPI_Finalize();
    return 0;
}
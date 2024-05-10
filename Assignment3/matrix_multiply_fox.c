#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

bool OUTPUT_TO_FILE = true;

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
            block[i * block_size + j] = temp[(block_row * block_size + i) * full_size + (block_col * block_size + j)];
        }
    }

    free(temp);
    fclose(file);
}

int get_rank(int row, int col, int grid_size)
{
    return ((row + grid_size) % grid_size) * grid_size + (col + grid_size) % grid_size;
}

int main(int argc, char **argv)
{

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


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

    int grid_size = sqrt(size); // processor matrix size
    if (grid_size * grid_size != size)
    {
        printf("The number of processes must be a perfect square!\n");
        MPI_Finalize();
        return 0;
    }

    if (dim % grid_size)
    {
        printf("dim must divide grid_size exactly !\n");
        MPI_Finalize();
        return 0;
    }

    int block_size = dim / grid_size;        // size of block
    int block_num = block_size * block_size; // count of element in each block

    double *matrixC = malloc(dim * dim * sizeof(double));
    double *tempC = malloc(dim * dim * sizeof(double));

    double *blockA = malloc(block_num * sizeof(double));
    double *blockB = malloc(block_num * sizeof(double));
    double *blockC = malloc(block_num * sizeof(double));
    double *tmpA = malloc(block_num * sizeof(double));
    double *tmpB = malloc(block_num * sizeof(double));

    int row = rank / grid_size; // position of processor in matrix
    int col = rank % grid_size;

    read_matrix_block("matrix_A.out", blockA, block_size, grid_size, row, col);
    read_matrix_block("matrix_B.out", blockB, block_size, grid_size, row, col);

    int sender_col = row;
    int rank_min, rank_max;

    double startTime = MPI_Wtime();

    for (int steps = 0; steps < grid_size; steps++)
    {
        // send blockA to other processor in the same line
        if (col == sender_col)
        {
            rank_min = get_rank(row, 0, grid_size);
            rank_max = get_rank(row, grid_size - 1, grid_size);
            for (int i = rank_min; i <= rank_max; i++)
            {
                if (i == rank)
                {
                    continue;
                }
                // send blockA to other processor in the same line
                MPI_Send(blockA, block_num, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            }
            memcpy(tmpA, blockA, block_num * sizeof(double));
        }
        else
        {
            MPI_Recv(tmpA, block_num, MPI_DOUBLE, get_rank(row, sender_col, grid_size), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // calculate
        for (int i = 0; i < block_size; i++)
        {
            for (int j = 0; j < block_size; j++)
            {
                double sum = blockC[i * block_size + j];
                for (int k = 0; k < block_size; k++)
                    sum += tmpA[i * block_size + k] * blockB[k * block_size + j];
                blockC[i * block_size + j] = sum;
            }
        }

        // update
        MPI_Sendrecv(blockB, block_num, MPI_DOUBLE, get_rank(row - 1, col, grid_size), 2, tmpB, block_num, MPI_DOUBLE, get_rank(row + 1, col, grid_size), 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        memcpy(blockB, tmpB, block_num * sizeof(double));
        sender_col = (sender_col + 1) % grid_size;
    }

    // Gathering the Result

    MPI_Gather(blockC, block_num, MPI_DOUBLE, tempC, block_num, MPI_DOUBLE, 0, MPI_COMM_WORLD);


    if (rank == 0)
    {
        for (int proc = 0; proc < size; proc++)
        {
            int grid_row = proc / grid_size;
            int grid_col = proc % grid_size;
            for (int i = 0; i < block_size; i++)
            {
                for (int j = 0; j < block_size; j++)
                {
                    int global_row = grid_row * block_size + i;
                    int global_col = grid_col * block_size + j;

                    int tempC_index = proc * block_num + i * block_size + j;

                    matrixC[global_row * dim + global_col] = tempC[tempC_index];
                }
            }
        }

        double endTime = MPI_Wtime();
        printf("Total time: %f seconds\n", endTime - startTime);

        if(OUTPUT_TO_FILE){
            FILE *file = fopen("matrix_C_FOX.out", "w");
            for (int i = 0; i < dim; i++)
            {
                for (int j = 0; j < dim; j++)
                {
                    fprintf(file, "%.4f ", matrixC[i * dim + j]);
                }
                fprintf(file, "\n");
            }
            fclose(file);
        }
    }

    free(matrixC);
    free(tempC);
    free(blockA);
    free(blockB);
    free(blockC);



    MPI_Finalize();
    return 0;
}
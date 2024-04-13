#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Assume CACHE_LINE_SIZE is 64 bytes
#define CACHE_LINE_SIZE 64

// Define a structure to keep local sums with padding
typedef struct padded_sum
{
    double sum;
    char padding[CACHE_LINE_SIZE - sizeof(double)];
} padded_sum_t;

double opt_local_sum(double *x, size_t size)
{
    int num_threads = omp_get_max_threads();                              // Obtain the max number of threads at runtime
    padded_sum_t *local_sum = malloc(num_threads * sizeof(padded_sum_t)); // Allocate array dynamically
    double global_sum = 0.0;

    // Initialize local sum array
    for (int i = 0; i < num_threads; ++i)
    {
        local_sum[i].sum = 0.0;
    }

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
#pragma omp for
        for (size_t i = 0; i < size; ++i)
        {
            local_sum[tid].sum += x[i];
        }
    }

    // Combine local sums into the global sum
    for (int i = 0; i < num_threads; ++i)
    {
        global_sum += local_sum[i].sum;
    }

    free(local_sum); // Free dynamically allocated memory
    return global_sum;
}

void generate_random(double *input, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        input[i] = rand() / (double)RAND_MAX;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <number of runs>\n", argv[0]);
        return 1;
    }

    int num_runs = atoi(argv[1]);
    if (num_runs <= 0)
    {
        fprintf(stderr, "Number of runs must be a positive integer\n");
        return 1;
    }

    size_t size = 10000000; // 10^7
    double *array = (double *)malloc(size * sizeof(double));
    generate_random(array, size);

    int thread_counts[] = {1, 32, 64, 128};
    int num_thread_counts = sizeof(thread_counts) / sizeof(thread_counts[0]);

    for (int j = 0; j < num_thread_counts; ++j)
    {
        int num_threads = thread_counts[j];
        double total_time = 0.0;

        omp_set_num_threads(num_threads); // Set the number of threads for this iteration

        for (int i = 0; i < num_runs; ++i)
        {
            double start_time = omp_get_wtime();
            double sum = opt_local_sum(array, size); // Calculate sum
            double end_time = omp_get_wtime();
            total_time += (end_time - start_time);
        }

        double average_time = total_time / num_runs;
        printf("Threads: %d, Average Time: %.6f seconds\n", num_threads, average_time);
    }

    free(array);

    return 0;
}
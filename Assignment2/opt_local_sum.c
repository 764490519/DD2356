#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Define CACHE_LINE_SIZE based on the architecture, usually 64 bytes
#define CACHE_LINE_SIZE 64
#define MAX_THREADS 128

// Create a struct that aligns each sum to a cache line to prevent false sharing
typedef struct padded_sum
{
    double sum;
    char padding[CACHE_LINE_SIZE - sizeof(double)];
} padded_sum_t;

double opt_local_sum(double *x, size_t size)
{
    padded_sum_t local_sum[MAX_THREADS];
    double global_sum = 0.0;

    // Initialize local sum array
    for (int i = 0; i < MAX_THREADS; i++)
    {
        local_sum[i].sum = 0.0;
    }

#pragma omp parallel num_threads(MAX_THREADS)
    {
        int tid = omp_get_thread_num();
#pragma omp for
        for (size_t i = 0; i < size; ++i)
        {
            local_sum[tid].sum += x[i];
        }
    }

    // Combine local sums into the global sum
    for (int i = 0; i < MAX_THREADS; ++i)
    {
        global_sum += local_sum[i].sum;
    }

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

        for (int i = 0; i < num_runs; ++i)
        {
            double start_time = omp_get_wtime();
            omp_set_num_threads(num_threads);
            opt_local_sum(array, size);
            double end_time = omp_get_wtime();
            total_time += (end_time - start_time);
        }

        double average_time = total_time / num_runs;
        printf("Threads: %d, Average Time: %.6f seconds\n", num_threads, average_time);
    }

    free(array);

    return 0;
}
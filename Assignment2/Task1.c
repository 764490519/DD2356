#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double serial_sum(double *x, size_t size)
{
    double sum_val = 0.0;

    for (size_t i = 0; i < size; i++)
    {
        sum_val += x[i];
    }

    return sum_val;
}

void generate_random(double *input, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        input[i] = rand() / (double)(RAND_MAX);
    }
}

int main()
{
    size_t size = 10000000; // 10^7
    double *array = malloc(size * sizeof(double));

    // 用随机值初始化数组
    generate_random(array, size);

    // 开始计时
    double start_time = omp_get_wtime();

    // 计算求和
    double sum = serial_sum(array, size);

    // 结束计时
    double end_time = omp_get_wtime();

    // 输出结果和性能
    printf("Sum: %f\n", sum);
    printf("Time taken: %f seconds\n", end_time - start_time);

    // 清理
    free(array);

    return 0;
}
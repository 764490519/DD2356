#!/bin/bash

# 定义测试的矩阵大小
matrix_dimension=256

# 进程数数组，每个元素都应该是一个完全平方数
# processor_counts=(1 4 9 16 25 36 49 64 100 144 196 225 256)
processor_sizes = (1)

# # 输出文件
# OUTPUT_FILE="mpi_performance_results.txt"

# # 清空输出文件
# echo "Testing MPI Matrix Multiplication Performance" > $OUTPUT_FILE
# echo "Matrix size: $MATRIX_SIZE" >> $OUTPUT_FILE
# echo "---------------------------------------------" >> $OUTPUT_FILE

# Loop over each processor size
for size in "${processor_sizes[@]}"
do

    # Run the MPI program and measure the time
    start_time=$(date +%s.%N)
    mpiexec -n $size ./fox.exe $matrix_dimension
    end_time=$(date +%s.%N)

    # Calculate and display the elapsed time
    elapsed_time=$(echo "$end_time - $start_time" | bc)
    echo "Elapsed time: $elapsed_time seconds"
    echo "-------------------------------------------------"
done
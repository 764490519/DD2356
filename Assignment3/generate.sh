#!/bin/bash

# Prompt user for input
read -p "Enter the matrix size (e.g., 4): " matrixSize

# Use default size if no input provided
if [[ -z "$matrixSize" ]]; then
    echo "No matrix size provided, using default size 4."
    matrixSize=4
fi

echo "Generating input matrices of size $matrixSize..."
gcc generate_matrices.c -o generate
./generate $matrixSize

echo "Generating standard matrix multiplication result for size $matrixSize..."
gcc matrix_multiply.c -o multiply
./multiply $matrixSize

echo "Done."
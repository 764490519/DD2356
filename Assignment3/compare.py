import numpy as np

def read_matrix(filename):
    with open(filename, 'r') as file:
        data = np.array([list(map(float, line.split())) for line in file if line.strip()])
    return data

def compare_matrices(file1, file2):
    matrixA = read_matrix(file1)
    matrixB = read_matrix(file2)

    if matrixA.shape != matrixB.shape:
        print("The matrices have different shapes and cannot be equal.")
        return False
    elif np.array_equal(matrixA, matrixB):
        print("The matrices are equal.")
        return True
    else:
        print("The matrices are not equal.")
        return False

# Specify the file paths
file_path1 = 'matrix_C_FOX.out'
file_path2 = 'matrix_C.out'

# Compare the matrices from the specified files
compare_matrices(file_path1, file_path2)
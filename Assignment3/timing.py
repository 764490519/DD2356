import subprocess
import re

# Set parameters
matrix_dimension = 1680
processor_sizes = [1, 4, 9, 16, 25, 36, 49, 64, 100, 144, 196, 225, 256]
output_file = "timing_results.txt"

# Prepare the output file
with open(output_file, "w") as f:
    f.write(f"Timing results for matrix dimension {matrix_dimension}\n")
    f.write("-------------------------------------------------\n")
    
    # Iterate over each processor size
    for size in processor_sizes:
        total_time = 0.0

        # Repeat the run 10 times for each processor size
        for _ in range(10):
            # Build and run the command
            command = f"srun -n {size} ./fox.exe {matrix_dimension}"
            result = subprocess.run(command, shell=True, universal_newlines=True, capture_output=True)
            
            # Extract runtime
            search = re.search(r"Total time: (\d+\.\d+) seconds", result.stdout)
            if search:
                runtime = float(search.group(1))
                total_time += runtime

        # Calculate the average runtime
        average_time = total_time / 10

        # Write the results to file
        f.write(f"Processor size: {size}\n")
        f.write(f"Average elapsed time: {average_time} seconds\n")
        f.write("-------------------------------------------------\n")
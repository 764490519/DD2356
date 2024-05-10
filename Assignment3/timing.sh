matrix_dimension=1680

processor_sizes=(1 4 9 16 25 36 49 64 100 144 196 225 256)

# Create or clear the output file before starting the loop
output_file="timing_results.txt"
echo "Timing results for matrix dimension $matrix_dimension" > "$output_file"
echo "-------------------------------------------------" >> "$output_file"

# Loop over each processor size
for size in "${processor_sizes[@]}"
do
    # Run the MPI program and capture its output
    output=$(srun -n $size ./fox.exe $matrix_dimension)
    echo "$output" # Optionally print the entire output to console

    # Extract the total runtime from the output
    runtime=$(echo "$output" | grep 'Total time:' | awk '{print $3}')
    
    # Display the processor size and extracted runtime
    echo "Processor size: $size" >> "$output_file"
    echo "Elapsed time: $runtime seconds" >> "$output_file"
    echo "-------------------------------------------------" >> "$output_file"
done
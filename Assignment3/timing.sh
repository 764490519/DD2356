matrix_dimension=1680

# processor_sizes=(1 4 9 16 25 36 49 64 100 144 196 225 256)
processor_sizes=(25 36 49 64 100 144 196 225 256)

# Create or clear the output file before starting the loop
output_file="timing_results.txt"
echo "Timing results for matrix dimension $matrix_dimension" > "$output_file"
echo "-------------------------------------------------" >> "$output_file"

# Loop over each processor size
for size in "${processor_sizes[@]}"
do
    # Initialize total time
    total_time=0

    # Run the MPI program 10 times
    for i in $(seq 1 10)
    do
        # Capture the output of the program
        output=$(srun -n $size ./fox.exe $matrix_dimension)
        
        # Extract the total runtime from the output
        runtime=$(echo "$output" | grep 'Total time:' | awk '{print $3}')

        # Sum up the runtimes
        total_time=$(echo "$total_time + $runtime" | bc)
    done

    # Calculate the average runtime
    average_time=$(echo "$total_time / 10" | bc -l)

    # Display the processor size and average runtime
    echo "Processor size: $size" >> "$output_file"
    echo "Average elapsed time: $average_time seconds" >> "$output_file"
    echo "-------------------------------------------------" >> "$output_file"
done
matrix_dimension=1680

processor_sizes = (1 4 9 16 25 36 49 64 100 144 196 225 256)

# Loop over each processor size
for size in "${processor_sizes[@]}"
do

    # Run the MPI program and measure the time
    start_time=$(date +%s.%N)
    srun -n $size ./fox.exe $matrix_dimension 
    end_time=$(date +%s.%N)

    # Calculate and display the elapsed time
    elapsed_time=$(echo "$end_time - $start_time" | bc)
    echo "Elapsed time: $elapsed_time seconds"
    echo "-------------------------------------------------"
done
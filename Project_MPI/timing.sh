#!/bin/bash -l
# The -l above is required to get the full environment with modules

# The name of the script is myjob
#SBATCH -J myjob
# Only 1 hour wall-clock time will be given to this job
#SBATCH -t 1:00:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH -e error_file.e

# Run the executable file 
# and write the output into my_output_file
for cpus in $(seq 1 128)
do
        export SLURM_NTASKS_PER_NODE=$cpus
        export MPICH_NO_BUFFER_ALIAS_CHECK=1
        srun -n $cpus ./a.exe 1000 >> execution_time.txt
done

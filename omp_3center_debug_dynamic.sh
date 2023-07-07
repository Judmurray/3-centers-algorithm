#!/bin/bash
#SBATCH -A cmda3634_rjh
#SBATCH -p normal_q
#SBATCH -t 00:10:00
#SBATCH --cpus-per-task=4
#SBATCH -o omp_3center_debug_dynamic.out

# Go to the directory where the job was submitted
cd $SLURM_SUBMIT_DIR

# Load the modules
module load matplotlib

# Build the executable
gcc -D DYNAMIC -D DEBUG -o omp_3center omp_3center.c -fopenmp

# OpenMP settings
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
export OMP_PROC_BIND=TRUE
export OMP_DYNAMIC=FALSE

# run omp_3center
cat cities162.dat | ./omp_3center 1
cat cities162.dat | ./omp_3center 2
cat cities162.dat | ./omp_3center 4

# The script will exit whether we give the "exit" command or not.
exit

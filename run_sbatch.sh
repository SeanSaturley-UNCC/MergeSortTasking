#!/bin/bash
#SBATCH --job-name=mergesort
#SBATCH --partition=Centaurus
#SBATCH --output=mergesort.out
#SBATCH --error=mergesort.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=00:10:00


module load gcc


make clean
make mergesort_seq
make mergesort_par


echo "Sequential MergeSort:"
./mergesort_seq 10000 42
./mergesort_seq 100000 42


echo "Parallel MergeSort:"
./mergesort_par 10000 2 1000 42
./mergesort_par 10000 4 1000 42
./mergesort_par 100000 2 1000 42
./mergesort_par 100000 4 1000 42

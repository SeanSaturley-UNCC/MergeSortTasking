# Parallel Merge Sort

This project implements **sequential and parallel merge sort** in C++ using omp_tasking.hpp.

## Files
- `mergesort_seq_nocopy.cpp` — Sequential merge sort  
- `mergesort_par_nocopy.cpp` — Parallel merge sort  
- `Makefile` — Build both programs  
- `run_sbatch.sh` — Submit script for Centaurus  

## Usage
Compile:
```bash
make mergesort_seq      # Compile sequential version
make mergesort_par      # Compile parallel version

```
## Running Locally
Sequentially
```bash
./mergesort_seq 1000000 42
```

Parallel
```bash
./mergesort_par 1000000 4 1000 42
```

## Running on Centaurus:
Submit the job using Slurm with:
```bash
sbatch run_sbatch.sh
```



# Parallel Merge Sort

This project implements **sequential and parallel merge sort** in C++ using a task-based OpenMP abstraction (`omp_tasking.hpp`).

## Files
- `mergesort_seq_nocopy.cpp` — Sequential merge sort  
- `mergesort_par_nocopy.cpp` — Parallel merge sort  
- `Makefile` — Build both programs  
- `run_sbatch.sh` — Submit script for Centaurus  

## Usage
Compile:
```bash
make



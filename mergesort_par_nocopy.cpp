// mergesort_par_nocopy.cpp
// Parallel mergesort (no-copy merge style) using tasking abstraction:
// doinparallel(num_threads, lambda), taskstart(lambda), taskwait()
#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include <functional>
#include <random>

#include "omp_tasking.hpp" // either provided by course or the fallback below

#define DEBUG 0

void generateMergeSortData (std::vector<int>& arr, size_t n, unsigned int seed) {
  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> dist(0, INT32_MAX);
  for (size_t i = 0; i < n; ++i) {
    arr[i] = dist(rng);
  }
}

void checkMergeSortResult (std::vector<int>& arr, size_t n) {
  bool ok = true;
  for (size_t  i=1; i<n; ++i)
    if (arr[i] < arr[i-1])
      ok = false;
  if(!ok)
    std::cerr<<"notok"<<std::endl;
}

void merge(int * arr, size_t l, size_t mid, size_t r, int* temp) {

#if DEBUG
  std::cout<<l<<" "<<mid<<" "<<r<<std::endl;
#endif

  // short circuits
  if (l == r) return;
  if (r - l == 1) {
    if (arr[l] > arr[r]) {
      int tmp = arr[l];
      arr[l] = arr[r];
      arr[r] = tmp;
    }
    return;
  }

  size_t i, j, k;
  size_t n = mid - l;

  // copy left half to temp
  for (i = 0; i < n; ++i)
    temp[i] = arr[l + i];

  i = 0;    // temp left half
  j = mid;  // right half (mid..r)
  k = l;    // write position

  // merge
  while (i < n && j <= r) {
     if (temp[i] <= arr[j]) {
       arr[k++] = temp[i++];
     } else {
       arr[k++] = arr[j++];
     }
  }

  // exhaust temp
  while (i < n) {
    arr[k++] = temp[i++];
  }
}

void mergesort_seq(int * arr, size_t l, size_t r, int* temp) {
  if (l < r) {
    size_t mid = (l + r) / 2;
    mergesort_seq(arr, l, mid, temp);
    mergesort_seq(arr, mid + 1, r, temp);
    merge(arr, l, mid + 1, r, temp);
  }
}

// Parallel mergesort implementation with threshold
void mergesort_par_impl(int * arr, size_t l, size_t r, int* temp, size_t threshold) {
  if (l >= r) return;
  size_t n = r - l + 1;
  if (n <= threshold) {
    mergesort_seq(arr, l, r, temp);
    return;
  }

  size_t mid = (l + r) / 2;

  // spawn tasks for left and right halves
  taskstart([&]() {
    mergesort_par_impl(arr, l, mid, temp, threshold);
  });
  taskstart([&]() {
    mergesort_par_impl(arr, mid + 1, r, temp, threshold);
  });

  // wait for children
  taskwait();

  // merge after both halves sorted
  merge(arr, l, mid + 1, r, temp);
}

// top-level parallel wrapper
void mergesort_par(int * arr, size_t n, int num_threads, size_t threshold) {
  if (n == 0) return;
  std::vector<int> temp(n);
  // use doinparallel to create parallel region
  doinparallel(num_threads, [&]() {
    mergesort_par_impl(arr, 0, n - 1, temp.data(), threshold);
  });
}

int main (int argc, char* argv[]) {
  // CLI: ./mergesort_par <n> [num_threads] [threshold] [seed]
  if (argc < 2) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> [num_threads] [threshold] [seed]"<<std::endl;
    return -1;
  }

  size_t n = atol(argv[1]);
  int num_threads = 4;
  size_t threshold = 1000;
  unsigned int seed = (unsigned int)time(nullptr);

  if (argc > 2) num_threads = atoi(argv[2]);
  if (argc > 3) threshold = (size_t)stoll(argv[3]);
  if (argc > 4) seed = (unsigned int)atoi(argv[4]);

  std::vector<int> arr(n);
  generateMergeSortData(arr, n, seed);

#if DEBUG
  for (size_t i=0; i<n; ++i) 
    std::cout<<arr[i]<<" ";
  std::cout<<std::endl;
#endif

  auto start = std::chrono::high_resolution_clock::now();

  mergesort_par(arr.data(), n, num_threads, threshold);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  // output runtime to stderr (same as your seq program)
  std::cerr<<elapsed_seconds.count()<<std::endl;

  checkMergeSortResult(arr, n);

#if DEBUG
  for (size_t i=0; i<n; ++i) 
    std::cout<<arr[i]<<" ";
  std::cout<<std::endl;
#endif

  return 0;
}

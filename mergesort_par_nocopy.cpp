#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <functional>
#include <random>
#include <string>   // for std::stoull
#include <ctime>    // for time(nullptr)

#include "omp_tasking.hpp"

#define DEBUG 0

void generateMergeSortData(std::vector<int>& arr, size_t n, unsigned int seed=0) {
    std::mt19937 rng(seed ? seed : (unsigned int)time(nullptr));
    std::uniform_int_distribution<int> dist(0, INT32_MAX);
    for (size_t i = 0; i < n; ++i) arr[i] = dist(rng);
}

void checkMergeSortResult(std::vector<int>& arr, size_t n) {
    for (size_t i = 1; i < n; ++i)
        if (arr[i] < arr[i - 1]) {
            std::cerr << "notok" << std::endl;
            return;
        }
}

void merge(int* arr, size_t l, size_t mid, size_t r, int* temp) {
    if (l >= r) return;
    size_t n = mid - l;
    for (size_t i = 0; i < n; ++i) temp[i] = arr[l + i];
    size_t i = 0, j = mid, k = l;
    while (i < n && j <= r) arr[k++] = (temp[i] <= arr[j]) ? temp[i++] : arr[j++];
    while (i < n) arr[k++] = temp[i++];
}

void mergesort_seq(int* arr, size_t l, size_t r, int* temp) {
    if (l < r) {
        size_t mid = (l + r) / 2;
        mergesort_seq(arr, l, mid, temp);
        mergesort_seq(arr, mid + 1, r, temp);
        merge(arr, l, mid + 1, r, temp);
    }
}

void mergesort_par_impl(int* arr, size_t l, size_t r, int* temp, size_t threshold) {
    if (l >= r) return;
    size_t n = r - l + 1;
    if (n <= threshold) {
        mergesort_seq(arr, l, r, temp);
        return;
    }

    size_t mid = (l + r) / 2;
    taskstart([&]() { mergesort_par_impl(arr, l, mid, temp, threshold); });
    taskstart([&]() { mergesort_par_impl(arr, mid + 1, r, temp, threshold); });
    taskwait();
    merge(arr, l, mid + 1, r, temp);
}

void mergesort_par(int* arr, size_t n, int num_threads, size_t threshold) {
    if (n == 0) return;
    std::vector<int> temp(n);
    doinparallel(num_threads, [&]() { mergesort_par_impl(arr, 0, n - 1, temp.data(), threshold); });
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <n> [num_threads] [threshold] [seed]" << std::endl;
        return -1;
    }

    size_t n = std::stoull(argv[1]);
    int num_threads = (argc > 2) ? std::stoi(argv[2]) : 4;
    size_t threshold = (argc > 3) ? std::stoull(argv[3]) : 1000;
    unsigned int seed = (argc > 4) ? (unsigned int)std::stoull(argv[4]) : (unsigned int)time(nullptr);

    std::vector<int> arr(n);
    generateMergeSortData(arr, n, seed);

    auto start = std::chrono::high_resolution_clock::now();
    mergesort_par(arr.data(), n, num_threads, threshold);
    auto end = std::chrono::high_resolution_clock::now();

    std::cerr << std::chrono::duration<double>(end - start).count() << std::endl;
    checkMergeSortResult(arr, n);

    return 0;
}

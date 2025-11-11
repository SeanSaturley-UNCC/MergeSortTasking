#ifndef OMP_TASKING_HPP
#define OMP_TASKING_HPP

#include <functional>
#include <omp.h>

inline void doinparallel(int num_threads, const std::function<void()> &f) {
    omp_set_num_threads(num_threads);
    #pragma omp parallel
    {
        #pragma omp single nowait
        f();
    }
}

inline void taskstart(const std::function<void()> &f) {
    #pragma omp task firstprivate(f)
    f();
}

inline void taskwait() {
    #pragma omp taskwait
}

#endif

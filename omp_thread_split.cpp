#include <omp.h>

#include <cassert>
#include <cstdio>

void print_parallel() {
#pragma omp parallel
    {
        printf("num_thread=%d, thread_num=%d\n", omp_get_num_threads(),
            omp_get_thread_num());
    }
}

int main() {
    printf("max_threads=%d\n", omp_get_max_threads());

    int max_threads = omp_get_max_threads();
    // int calculate_threads = 4;
    int calculate_threads = max_threads - 1;
    assert(calculate_threads + 1 <= max_threads);

    int root_num_threads = (max_threads - 1) / calculate_threads + 1;
    printf("root_num_threads: %d\n", root_num_threads);

    omp_set_nested(1);
    omp_set_num_threads(root_num_threads);
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        if (tid == 0) {
            omp_set_num_threads(1);
            print_parallel();
        } else {
            omp_set_num_threads(calculate_threads);
            print_parallel();
        }
    }
    omp_set_num_threads(max_threads);
    omp_set_nested(0);

    return 0;
}

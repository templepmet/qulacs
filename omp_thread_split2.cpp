#include <omp.h>

#include <cassert>
#include <chrono>
#include <cstdio>
#include <vector>

using namespace std;

void print_parallel() {
    printf("single: num_thread=%d, thread_num=%d\n", omp_get_num_threads(),
        omp_get_thread_num());
#pragma omp parallel
    printf("num_thread=%d, thread_num=%d\n", omp_get_num_threads(),
        omp_get_thread_num());
}

void heavy_func(int id, int tid) {
    int n = 2e9;

    double start, end;
    start = omp_get_wtime();

    long long sum = 0;
#pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < n; ++i) {
        sum += i;
    }

    end = omp_get_wtime();
    double time = end - start;

    printf(
        "i=%d, tid=%d sum=%lld, "
        "time=%lf[s], num_threads=%d, thread_num=%d, "
        "max_threads=%d\n",
        id, tid, sum, time, omp_get_num_threads(), omp_get_thread_num(),
        omp_get_max_threads());
}

int main() {
    printf("max_threads=%d\n", omp_get_max_threads());

    int max_threads = omp_get_max_threads();

    // heavy_func();
    // heavy_func();

    omp_set_nested(1);
    omp_set_num_threads(2);

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < 10; ++i) {
        int tid = omp_get_thread_num();
        double time;
        if (tid == 0) {  // 1 thread
            omp_set_num_threads(2);
            heavy_func(i, tid);
        } else {  // multi thread
            omp_set_num_threads(max_threads - 2);
            heavy_func(i, tid);
            // omp_set_num_threads(1);
        }
    }

    omp_set_num_threads(max_threads);
    omp_set_nested(0);

    return 0;
}

// num_threads(2)
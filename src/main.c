#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
    #include <sched.h>
    #include <errno.h>
#endif

#include <stdint.h>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

// benchmark includes
#include "../include/int.h"
#include "../include/fp.h"
#include "../include/vect.h"
#include "../include/mem.h"

#define N_BENCHMARKS 22

typedef enum {
    bench_latency_int32         = 0,
    bench_throughput_int32      = 1,
    bench_latency_int64         = 2,
    bench_throughput_int64      = 3,

    bench_latency_fp32          = 4,
    bench_throughput_fp32       = 5,
    bench_latency_fp64          = 6,
    bench_throughput_fp64       = 7,

    bench_latency_v128          = 8,
    bench_throughput_v128       = 9,
    bench_latency_v256          = 10,
    bench_throughput_v256       = 11,
    bench_latency_v512          = 12,
    bench_throughput_v512       = 13,

    bench_latency_mem_rd_l1     = 14,
    bench_throughput_mem_rd_l1  = 15,
    bench_latency_mem_rd_l2     = 16,
    bench_throughput_mem_rd_l2  = 17,
    bench_latency_mem_rd_l3     = 18,
    bench_throughput_mem_rd_l3  = 19,
    bench_latency_mem_rd_mem    = 20,
    bench_throughput_mem_rd_mem = 21,

    bench_none
} benchMode_t;

typedef struct {
    const char* flag;
    const char* nIter;
    const char* benchText;
    benchMode_t mode;
    int (*benchFunc)(void);
} flagMapping_t;


int main(int argc, char** argv) {
    if (argc <= 2) {
        printf("Usage: %s [mode] coreID_1 coreID_2 ... coreID_n\n", argv[0]);
        printf("\tmodes: INT:  -i32l\t -i32t\t -i64l\t -i64t\n");
        printf("\t       FP:   -f32l\t -f32t\t -f64l\t -f64t\n");
        printf("\t       VECT: -v128l\t-v128t\t-v256l\t-v256t\n");
        printf("\t             -v512l\t-v512t\n");
        printf("\t       MEM:  -l1rl\t -l1rt\t -l2rl\t -l2rt\n");
        printf("\t             -l3rl\t -l3rt\t -mrl\t  -mrt\n");
        return 1;
    }

    flagMapping_t flagMaps[N_BENCHMARKS] = {
        // int
        {"-i32l", "10^9",  "int32 latency",          bench_latency_int32,         loadLatency_int32},
        {"-i32t", "10^9",  "int32 throughput",       bench_throughput_int32,      loadThroughput_int32},
        {"-i64l", "10^10", "int64 latency",          bench_latency_int64,         loadLatency_int64},
        {"-i64t", "10^10", "int64 throughput",       bench_throughput_int64,      loadThroughput_int64}, 

        // fp
        {"-f32l", "10^9", "float32 latency",         bench_latency_fp32,          loadLatency_fp32},
        {"-f32t", "10^9", "float32 throughput",      bench_throughput_fp32,       loadThroughput_fp32},
        {"-f64l", "10^9", "float64 latency",         bench_latency_fp64,          loadLatency_fp64},
        {"-f64t", "10^9", "float64 throughput",      bench_throughput_fp64,       loadThroughput_fp64},

        // vect
        {"-v128l", "10^9", "vect128 latency",        bench_latency_v128,          loadLatency_v128},
        {"-v128t", "10^9", "vect128 throughput",     bench_throughput_v128,       loadThroughput_v128},
        {"-v256l", "10^9", "vect256 latency",        bench_latency_v256,          loadLatency_v256},
        {"-v256t", "10^9", "vect256 througput",      bench_throughput_v256,       loadThroughput_v256},
        {"-v512l", "10^9", "vect512 latency",        bench_latency_v512,          loadLatency_v512},
        {"-v512t", "10^9", "vect512 throughput",     bench_throughput_v512,       loadThroughput_v512},

        // memory
        {"-l1rl", "10^10", "L1 read latency",        bench_latency_mem_rd_l1,     loadLatency_mem_rd_l1},
        {"-l1rt", "10^7",  "L1 read throughput",     bench_throughput_mem_rd_l1,  loadThroughput_mem_rd_l1},
        {"-l2rl", "10^9",  "L2 read latency",        bench_latency_mem_rd_l2,     loadLatency_mem_rd_l2},
        {"-l2rt", "10^5",  "L2 read throughput",     bench_throughput_mem_rd_l2,  loadThroughput_mem_rd_l2},
        {"-l3rl", "10^9",  "L3 read latency",        bench_latency_mem_rd_l3,     loadLatency_mem_rd_l3},
        {"-l3rt", "10^4",  "L3 read throughput",     bench_throughput_mem_rd_l3,  loadThroughput_mem_rd_l3},
        {"-mrl",  "10^7",  "memory read latency",    bench_latency_mem_rd_mem,    loadLatency_mem_rd_spill},
        {"-mrt",  "10^2",  "memory read throughput", bench_throughput_mem_rd_mem, loadThroughput_mem_rd_spill},
    };

    benchMode_t mode = bench_none;
    for (int i = 0; i < N_BENCHMARKS; i++){
        if (strcmp(flagMaps[i].flag, argv[1]) == 0) mode = flagMaps[i].mode;
    }

    if (mode == bench_none) {
        printf("Invalid arguments. Select a proper benchmark mode\n");
        return 1;  
    } else {
        printf("Benching %s with %s iterations\n", flagMaps[mode].benchText, flagMaps[mode].nIter);
    }

    // parse all cores
    static const int nIDs = 64; // MSVC hates VLAs
    int ids[nIDs];

    for (int i = 2; i < argc; i++) {
        ids[i - 2] = atoi(argv[i]);
    }

    for (int i = 0; i < argc - 2; i++) {
    int core_id = ids[i];

    #ifdef _WIN32
    // ================= WINDOWS =================

    // Core 0 = 1, Core 21 = 2^21
    DWORD_PTR mask = (DWORD_PTR)1ULL << core_id;

    HANDLE hThread = GetCurrentThread();

    DWORD_PTR prev_affinity = SetThreadAffinityMask(hThread, mask);

    if (prev_affinity == 0) {
        DWORD error = GetLastError();
        printf("Failed to set affinity to Core %d. Error Code: %lu\n", core_id, error);
        printf("(You might be targeting a core that doesn't exist?)\n");
        return 1;
    }

    #else
    // ================= POSIX =================

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    pthread_t thread = pthread_self();

    int result = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

    if (result != 0) {
        printf("Failed to set affinity to Core %d. Error Code: %d\n", core_id, result);
        printf("(You might be targeting a core that doesn't exist or lack permissions?)\n");
        return 1;
    }

    #endif

        printf("Logical Core %d\n", ids[i]);

        clock_t start = clock();

        flagMaps[mode].benchFunc();

        clock_t end = clock();
        double timeSpent = (double)(end-start) / CLOCKS_PER_SEC; // for seconds

        printf("\t\ttime: %.3fs\n\n", timeSpent);
    }

    return 0;
}


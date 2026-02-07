// INTEGER BENCHMARKS
#include <stdint.h>

int loadThroughput_int32() {
    uint32_t scratch1 = 23467;
    uint32_t scratch2 = 23468;
    uint32_t scratch3 = 23469;
    uint32_t scratch4 = 23470;
    uint32_t scratch5 = 23471;
    uint32_t scratch6 = 23472;
    uint32_t scratch7 = 23473;
    uint32_t scratch8 = 23474;

    for (uint32_t i = 0; i < 1000000000; i++) {
        scratch1 = scratch1 * 6793005 + 1;
        scratch2 = scratch2 * 6793006 + 1;
        scratch3 = scratch3 * 6793007 + 1;
        scratch4 = scratch4 * 6793008 + 1;
        scratch5 = scratch5 * 6793009 + 1;
        scratch6 = scratch6 * 6793010 + 1;
        scratch7 = scratch7 * 6793011 + 1;
        scratch8 = scratch8 * 6793012 + 1;
    }

    volatile uint32_t sink;
    sink = scratch1;
    sink = scratch2;
    sink = scratch3;
    sink = scratch4;
    sink = scratch5;
    sink = scratch6;
    sink = scratch7;
    sink = scratch8;
    
    return 0;
}

int loadLatency_int32() {
    uint32_t scratch = 23467;

    for (uint32_t i = 0; i < 1000000000; i++) {
        scratch = scratch * 6793005 + 1;
    }

    volatile uint32_t sink = scratch;

    return 0;
}

int loadThroughput_int64() {
    // 8 scratch values, maximize registers, minimize memory spills
    uint64_t scratch1 = 23467;
    uint64_t scratch2 = 23468;
    uint64_t scratch3 = 23469;
    uint64_t scratch4 = 23470;
    uint64_t scratch5 = 23471;
    uint64_t scratch6 = 23472;
    uint64_t scratch7 = 23473;
    uint64_t scratch8 = 23474;

    for (uint64_t i = 0; i < 10000000000; i++) {
        scratch1 = scratch1 * 6364136223846793005ULL + 1;
        scratch2 = scratch2 * 6364136223846793006ULL + 1;
        scratch3 = scratch3 * 6364136223846793007ULL + 1;
        scratch4 = scratch4 * 6364136223846793008ULL + 1;
        scratch5 = scratch5 * 6364136223846793009ULL + 1;
        scratch6 = scratch6 * 6364136223846793010ULL + 1;
        scratch7 = scratch7 * 6364136223846793011ULL + 1;
        scratch8 = scratch8 * 6364136223846793012ULL + 1;
    }

    volatile uint64_t sink;
    sink = scratch1;
    sink = scratch2;
    sink = scratch3;
    sink = scratch4;
    sink = scratch5;
    sink = scratch6;
    sink = scratch7;
    sink = scratch8;
    
    return 0;
}

int loadLatency_int64() {
    uint64_t scratch = 23467;

    for (uint64_t i = 0; i < 10000000000; i++) {
        scratch = scratch * 6364136223846793005ULL + 1;
    }

    volatile uint64_t sink = scratch;

    return 0;
}


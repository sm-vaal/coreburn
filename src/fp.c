#include <stdint.h>

// FLOATING POINT BENCHMARKS

int loadThroughput_fp32() {
    // 8 scratch values, maximize registers, minimize memory spills
    // NOTE: it CAN vectorize using fp MAX instructions. GREAT
    float scratch1 = 23467.23467f;
    float scratch2 = 23468.23468f;
    float scratch3 = 23469.23469f;
    float scratch4 = 23470.23470f;
    float scratch5 = 23471.23471f;
    float scratch6 = 23472.23472f;
    float scratch7 = 23473.23473f;
    float scratch8 = 23474.23474f;

    for (uint32_t i = 0; i < 1000000000; i++) {
        scratch1 = scratch1 * 6793005.0127f + 1.03f;
        scratch2 = scratch2 * 6793006.0127f + 1.03f;
        scratch3 = scratch3 * 6793007.0127f + 1.03f;
        scratch4 = scratch4 * 6793008.0127f + 1.03f;
        scratch5 = scratch5 * 6793009.0127f + 1.03f;
        scratch6 = scratch6 * 6793010.0127f + 1.03f;
        scratch7 = scratch7 * 6793011.0127f + 1.03f;
        scratch8 = scratch8 * 6793012.0127f + 1.03f;
    }

    volatile float sink;
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

int loadLatency_fp32() {
    float scratch = 23467.23467;

    for (uint32_t i = 0; i < 1000000000; i++) {
        scratch = scratch * 6793005.0127f + 1.03f;
    }

    volatile float sink = scratch;

    return 0;
}

int loadThroughput_fp64() {
    // 8 scratch values, maximize registers, minimize memory spills
    double scratch1 = 23467.23467;
    double scratch2 = 23468.23468;
    double scratch3 = 23469.23469;
    double scratch4 = 23470.23470;
    double scratch5 = 23471.23471;
    double scratch6 = 23472.23472;
    double scratch7 = 23473.23473;
    double scratch8 = 23474.23474;

    for (uint32_t i = 0; i < 1000000000; i++) {
        scratch1 = scratch1 * 6793005.0127 + 1.03;
        scratch2 = scratch2 * 6793006.0127 + 1.03;
        scratch3 = scratch3 * 6793007.0127 + 1.03;
        scratch4 = scratch4 * 6793008.0127 + 1.03;
        scratch5 = scratch5 * 6793009.0127 + 1.03;
        scratch6 = scratch6 * 6793010.0127 + 1.03;
        scratch7 = scratch7 * 6793011.0127 + 1.03;
        scratch8 = scratch8 * 6793012.0127 + 1.03;
    }

    volatile double sink;
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

int loadLatency_fp64() {
    double scratch = 23467.23467;

    for (uint32_t i = 0; i < 1000000000; i++) {
        scratch = scratch * 6793005.0127 + 1.03;
    }

    volatile double sink = scratch;

    return 0;
}
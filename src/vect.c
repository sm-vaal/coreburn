#include <stdint.h>

// VECTOR (AVX or NEON) BENCHMARKS
// NOTE: compiler dependent, TAKE CARE OF FLAGS, HELPS (e.g. -mavx2 or -march=native)
// Avoiding inline assembly for portability

typedef struct {
    uint32_t v[4];
} vec128_t __attribute__((aligned(16)));

typedef struct {
    uint32_t v[8];
} vec256_t __attribute__((aligned(32)));

typedef struct {
    uint32_t v[16];
} vec512_t __attribute__((aligned(64)));


int loadLatency_v128() {

    vec128_t vect1 = {7, 45, 90, 12};
    const vec128_t vect2 = {8, 46, 91, 13};
    const vec128_t vect3 = {9, 47, 92, 14};

    #pragma omp simd
    for (uint32_t i = 0; i < 1000000000; i++) {
        for (uint32_t j = 0; j < 4; j++) { // pray it vectorizes
            vect1.v[j] = vect1.v[j] * vect2.v[j] + vect3.v[j];
        }
    }

    volatile vec128_t sink = vect1;
    return 0;
}

int loadThroughput_v128() {

    vec128_t vect1 = {7,  45, 90, 12};
    vec128_t vect2 = {8,  46, 91, 13};
    vec128_t vect3 = {9,  47, 92, 14};
    vec128_t vect4 = {10, 45, 90, 12};
    vec128_t vect5 = {11, 46, 91, 13};
    vec128_t vect6 = {12, 47, 92, 14};
    vec128_t vect7 = {13, 45, 90, 12};
    vec128_t vect8 = {14, 46, 91, 13};

    const vec128_t vecMul = {2, 6, 255, 127};
    const vec128_t vecAdd = {1, 2,   3,   4};

    // #pragma GCC unroll 4   // force unrolling
    #pragma omp simd // try to use simd
    for (uint32_t i = 0; i < 1000000000; i++) {
        
        for (uint32_t j = 0; j < 4; j++) { // pray it vectorizes
            vect1.v[j] = vect1.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect2.v[j] = vect2.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect3.v[j] = vect3.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect4.v[j] = vect4.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect5.v[j] = vect5.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect6.v[j] = vect6.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect7.v[j] = vect7.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect8.v[j] = vect8.v[j] * vecMul.v[j] + vecAdd.v[j];
        }
    }

    volatile vec128_t sink = vect1;
    sink = vect2;
    sink = vect3;
    sink = vect4;
    sink = vect5;
    sink = vect6;
    sink = vect7;
    sink = vect8;
    return 0;
}



int loadLatency_v256() {
    vec256_t vect1 = {7, 45, 90, 12, 255, 120, 1, 175};
    const vec256_t vect2 = {8, 46, 91, 13, 0,   121, 2, 176};
    const vec256_t vect3 = {9, 47, 92, 14, 1,   122, 3, 177};

    #pragma omp simd
    for (uint32_t i = 0; i < 1000000000; i++) {
        for (uint32_t j = 0; j < 8; j++) { 
            vect1.v[j] = vect1.v[j] * vect2.v[j] + vect3.v[j];
        }
    }

    volatile vec256_t sink = vect1;
    return 0;
    
}

int loadThroughput_v256() {

    vec256_t vect1 = {7, 45, 90, 12, 255, 120, 1, 175};
    vec256_t vect2 = {8, 46, 91, 13, 0,   121, 2, 176};
    vec256_t vect3 = {9, 47, 92, 14, 1,   122, 3, 177};
    vec256_t vect4 = {10, 45, 90, 12, 255, 120, 1, 175};
    vec256_t vect5 = {11, 46, 91, 13, 255, 120, 1, 175};
    vec256_t vect6 = {12, 47, 92, 14, 255, 120, 1, 175};
    vec256_t vect7 = {13, 45, 90, 12, 255, 120, 1, 175};
    vec256_t vect8 = {14, 46, 91, 13, 255, 120, 1, 175};

    const vec256_t vecMul = {2, 6, 255, 127, 1, 2,   3,  4,};
    const vec256_t vecAdd = {1, 2,   3,   4, 2, 6, 255, 127};

    // #pragma GCC unroll 8   // force unrolling
    #pragma omp simd
    for (uint32_t i = 0; i < 1000000000; i++) {
        for (uint32_t j = 0; j < 8; j++) { 
            vect1.v[j] = vect1.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect2.v[j] = vect2.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect3.v[j] = vect3.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect4.v[j] = vect4.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect5.v[j] = vect5.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect6.v[j] = vect6.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect7.v[j] = vect7.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect8.v[j] = vect8.v[j] * vecMul.v[j] + vecAdd.v[j];
        }
    }

    volatile vec256_t sink = vect1;
    sink = vect2;
    sink = vect3;
    sink = vect4;
    sink = vect5;
    sink = vect6;
    sink = vect7;
    sink = vect8;
    return 0;

}



int loadLatency_v512() {
    vec512_t vect1 = {7, 45, 90, 12, 255, 120, 1, 175, 9, 47, 92, 14, 1,   122, 3, 177};
    const vec512_t vect2 = {8, 46, 91, 13, 0,   121, 2, 176, 7, 45, 90, 12, 255, 120, 1, 175};
    const vec512_t vect3 = {9, 47, 92, 14, 1,   122, 3, 177, 8, 46, 91, 13, 0,   121, 2, 176};

    #pragma omp simd
    for (uint32_t i = 0; i < 1000000000; i++) {
        for (uint32_t j = 0; j < 16; j++) { 
            vect1.v[j] = vect1.v[j] * vect2.v[j] + vect3.v[j];
        }
    }

    volatile vec512_t sink = vect1;
    return 0;
    
}


int loadThroughput_v512() {
    // NOTE: This WILL shit itself if no AVX-512 support, won't autovectorize, and will be a memory benchmark really
    vec512_t vect1 = {7, 45, 90, 12, 255, 120, 1, 175, 14, 46, 91, 13, 255, 120, 1, 175};
    vec512_t vect2 = {8, 46, 91, 13, 0,   121, 2, 176, 7, 45, 90, 12, 255, 120, 1, 175};
    vec512_t vect3 = {9, 47, 92, 14, 1,   122, 3, 177, 8, 46, 91, 13, 0,   121, 2, 176};
    vec512_t vect4 = {10, 45, 90, 12, 255, 120, 1, 175, 9, 47, 92, 14, 1,   122, 3, 177};
    vec512_t vect5 = {11, 46, 91, 13, 255, 120, 1, 175, 10, 45, 90, 12, 255, 120, 1, 175};
    vec512_t vect6 = {12, 47, 92, 14, 255, 120, 1, 175, 11, 46, 91, 13, 255, 120, 1, 175};
    vec512_t vect7 = {13, 45, 90, 12, 255, 120, 1, 175, 12, 47, 92, 14, 255, 120, 1, 175};
    vec512_t vect8 = {14, 46, 91, 13, 255, 120, 1, 175, 13, 45, 90, 12, 255, 120, 1, 175};

    const vec512_t vecMul = {2, 6, 255, 127, 1, 2,   3,  4, 1, 2,   3,   4, 2, 6, 255, 127};
    const vec512_t vecAdd = {1, 2,   3,   4, 2, 6, 255, 127, 2, 6, 255, 127, 1, 2,   3,  4};

    // #pragma GCC unroll 16   // force unrolling
    #pragma omp simd
    for (uint32_t i = 0; i < 1000000000; i++) {
        for (uint32_t j = 0; j < 16; j++) { 
            
            vect1.v[j] = vect1.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect2.v[j] = vect2.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect3.v[j] = vect3.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect4.v[j] = vect4.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect5.v[j] = vect5.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect6.v[j] = vect6.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect7.v[j] = vect7.v[j] * vecMul.v[j] + vecAdd.v[j];
            vect8.v[j] = vect8.v[j] * vecMul.v[j] + vecAdd.v[j];
        }
    }

    volatile vec512_t sink = vect1;
    sink = vect2;
    sink = vect3;
    sink = vect4;
    sink = vect5;
    sink = vect6;
    sink = vect7;
    sink = vect8;
    return 0;

}
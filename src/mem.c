#include <stdint.h>
#include <stdlib.h>

// vectors are GREAT for saturating r/w ports
typedef struct {
    uint32_t v[8];
} vec256_t __attribute__((aligned(32)));

// modify this for different CPUs
// In my case, meteor lake:
#define KIB 1024
#define L1_SIZE_BYTES   48 * KIB         // 48KiB L1 for data
#define L2_SIZE_BYTES   2 *   1024 * KIB // 2MiB L2 for data/instructions
#define L3_SIZE_BYTES   24 *  1024 * KIB // 24MiB shared L3
#define MEM_SPILL_BYTES 512 * 1024 * KIB // 512MiB to spill on purpose

// ##### READ BENCHMARKS #####
//  --- Throughput

int loadThroughput_mem_rd_l1(){
    // To analyze throughput, we want to be sequential
    // let the prefetcher do its thing
    
    // to saturate the data port, we should try to use vector ops, int64 isn't enough
    // modern cores are good at writing long words

    // NOTE: to actually calculate write throughput in GiB/s, take into account we are using 256B writes (likely AVX2)
    // using half cache size to prevent ANY spill to memory
    uint64_t size = (L1_SIZE_BYTES / sizeof(vec256_t)) / 2; // SIZE IS IN 256-bit VECTORS, NOT IN BYTES

    vec256_t buf[size];
    vec256_t magicNum = {0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE};

    // first, we write. negligible compared to the reads, and ensures zero-page optimizations don't affect us
    for (int i = 0; i < size; i++) {
        buf[i] = magicNum;
    }


    // most data in the buffer should be in L1
    vec256_t acc1;
    vec256_t acc2;
    vec256_t acc3;
    vec256_t acc4;

    #pragma GCC unroll 8
    for (int j = 0; j < 10000000; j++){
        for (int i = 0; i < size; i++) {
            for (int k = 0; k < 8; k++) {
                acc1.v[k] += buf[i].v[k]; // enable port saturation
                acc2.v[k] += buf[i].v[k];
                acc3.v[k] += buf[i].v[k];
                acc4.v[k] += buf[i].v[k];
            }
        }
    }

    volatile vec256_t sink = acc1;
    sink = acc2;
    sink = acc3;
    sink = acc4;

    return 0;
}

int loadThroughput_mem_rd_l2(){
    // for L2, the stack is not enough, we should use the heap
    uint64_t size = (L2_SIZE_BYTES / sizeof(vec256_t)) / 2;

    vec256_t* buf = malloc(size * sizeof(vec256_t));
    if (!buf) return -1;
    vec256_t magicNum = {0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE};

    // first, we write
    for (int i = 0; i < size; i++) {
        buf[i] = magicNum;
    }

    vec256_t acc1;
    vec256_t acc2;
    vec256_t acc3;
    vec256_t acc4;

    #pragma GCC unroll 8
    for (int j = 0; j < 100000; j++){
        for (int i = 0; i < size; i++) {
            for (int k = 0; k < 8; k++) {
                acc1.v[k] += buf[i].v[k]; // enable port saturation
                acc2.v[k] += buf[i].v[k];
                acc3.v[k] += buf[i].v[k];
                acc4.v[k] += buf[i].v[k];
            }
        }
    }

    volatile vec256_t sink = acc1;
    sink = acc2;
    sink = acc3;
    sink = acc4;

    free(buf);
    return 0;
}

int loadThroughput_mem_rd_l3(){
    // for L2, the stack is not enough, we should use the heap
    uint64_t size = (L3_SIZE_BYTES / sizeof(vec256_t)) / 2;

    vec256_t* buf = malloc(size * sizeof(vec256_t));
    if (!buf) return -1;
    vec256_t magicNum = {0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE};

    // first, we write
    for (int i = 0; i < size; i++) {
        buf[i] = magicNum;
    }

    vec256_t acc1;
    vec256_t acc2;
    vec256_t acc3;
    vec256_t acc4;

    #pragma GCC unroll 8
    for (int j = 0; j < 10000; j++){
        for (int i = 0; i < size; i++) {
            for (int k = 0; k < 8; k++) {
                acc1.v[k] += buf[i].v[k]; // enable port saturation
                acc2.v[k] += buf[i].v[k];
                acc3.v[k] += buf[i].v[k];
                acc4.v[k] += buf[i].v[k];
            }
        }
    }

    volatile vec256_t sink = acc1;
    sink = acc2;
    sink = acc3;
    sink = acc4;

    free(buf);
    return 0;
}

int loadThroughput_mem_rd_spill(){
    // for L2, the stack is not enough, we should use the heap
    uint64_t size = (MEM_SPILL_BYTES / sizeof(vec256_t));

    vec256_t* buf = malloc(size * sizeof(vec256_t));
    if (!buf) return -1;
    vec256_t magicNum = {0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE, 0xDEADBEEF, 0x00C0FFEE};

    // first, we write
    for (int i = 0; i < size; i++) {
        buf[i] = magicNum;
    }

    vec256_t acc1;
    vec256_t acc2;
    vec256_t acc3;
    vec256_t acc4;

    #pragma GCC unroll 8
    for (int j = 0; j < 100; j++){
        for (int i = 0; i < size; i++) {
            for (int k = 0; k < 8; k++) {
                acc1.v[k] += buf[i].v[k]; // enable port saturation
                acc2.v[k] += buf[i].v[k];
                acc3.v[k] += buf[i].v[k];
                acc4.v[k] += buf[i].v[k];
            }
        }
    }

    volatile vec256_t sink = acc1;
    sink = acc2;
    sink = acc3;
    sink = acc4;

    free(buf);
    return 0;
}


//  --- Latency
// MSVC uses 16-bit ints in rand(), so this provides portability
uint64_t rand64() {
    static uint64_t xorShiftState = 0xDEADBEEF00C0FFEE;
    xorShiftState ^= xorShiftState << 13;
    xorShiftState ^= xorShiftState >> 17;
    xorShiftState ^= xorShiftState << 5;
    return xorShiftState;
}

// Used to generate random access patterns
int setupPointerChase(void** buffer, size_t numElements) {
    size_t* indices =  (size_t*) malloc(numElements * sizeof(size_t));
    for (size_t i = 0; i < numElements; i++) indices[i] = i;

    // Fisher-yates shuffle
    for (size_t i = 0; i < numElements; i++) {
        size_t j = rand64() % (i + 1); // get a random num to swap
        size_t tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    // link the pointers based on the shuffle
    for (size_t i = 0; i < numElements - 1; i++) {
        buffer[indices[i]] = &buffer[indices[i + 1]];
    }
    // close the loop
    buffer[indices[numElements - 1]] = &buffer[indices[0]];
    free(indices);
}

int loadLatency_mem_rd_l1() {
    uint64_t size = (L1_SIZE_BYTES / sizeof(void*)) / 2;

    void** buf = malloc(size * sizeof(void*));
    if (!buf) return -1;

    setupPointerChase(buf, size);

    // ensure data is actually in cache
    void* p = buf[0];
    for (int i = 0; i < size * 2; i++) {
        p = *(void**)p;
    }

    for (size_t i = 0; i < 10000000000; i += 10) {
        p = *(void**)p; // small manual unroll
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
    }

    volatile uintptr_t result = (uintptr_t)p;
    free(buf);
    return 0;
}

int loadLatency_mem_rd_l2() {
    uint64_t size = (L2_SIZE_BYTES / sizeof(void*)) / 2;

    void** buf = malloc(size * sizeof(void*));
    if (!buf) return -1;

    setupPointerChase(buf, size);

    // ensure data is actually in cache
    void* p = buf[0];
    for (int i = 0; i < size * 2; i++) {
        p = *(void**)p;
    }

    for (size_t i = 0; i < 1000000000; i += 10) {
        p = *(void**)p; // small manual unroll
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
    }

    volatile uintptr_t result = (uintptr_t)p;
    free(buf);
    return 0;
}

int loadLatency_mem_rd_l3() {
    uint64_t size = (L3_SIZE_BYTES / sizeof(void*)) / 2;

    void** buf = malloc(size * sizeof(void*));
    if (!buf) return -1;

    setupPointerChase(buf, size);

    // ensure data is actually in cache
    void* p = buf[0];
    for (int i = 0; i < size * 2; i++) {
        p = *(void**)p;
    }

    for (size_t i = 0; i < 1000000000; i += 10) {
        p = *(void**)p; // small manual unroll
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
    }

    volatile uintptr_t result = (uintptr_t)p;
    free(buf);
    return 0;
}

int loadLatency_mem_rd_spill() {
    uint64_t size = (MEM_SPILL_BYTES / sizeof(void*));

    void** buf = malloc(size * sizeof(void*));
    if (!buf) return -1;

    setupPointerChase(buf, size);

    void* p = buf[0];

    for (size_t i = 0; i < 10000000; i += 10) {
        p = *(void**)p; // small manual unroll
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
        p = *(void**)p;
    }

    volatile uintptr_t result = (uintptr_t)p;
    free(buf);
    return 0;
}
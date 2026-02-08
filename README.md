# Coreburn
Tiny, cross platform microarchitecture benchmark suite.

## The why, the what, and the how:
Coreburn is a simple, CLI-based program to roughly compare the runtime of different instruction sequences. 
It excels in comparing different types of cores in heterogeneous architectures (e.g., intel meteor lake) 
or the basic performance between different CPUs.

It's **not at all meant for extremely accurate benchmarking**, or real-world use cases, the tests are
built to stress a single particular part of the system, like the ALU, FPU, read/write ports or cache.

It's also fully cross platform (windows/linux, x86/ARM), as it avoids intrinsics or inline assembly. 
However, it must be noted that **the compiler used will have a huge effect on performance**. 
I tested it with MSYS2's gcc, so your mileage may vary.

### Why not intrinsics?
**Portability**, portability, and portability.

A goal of this project was to support GCC and MSVC for compiling, and x86-64 and arm64 architectures.
Using intrinsics or inline asm would break this. Technically more accurate, but modern compilers are smart.
If you really need it, either: 

* inspect the produced asssembly: ```gcc -S -O2 -march=native yourFile.c```
* or use objdump: ```-d -M intel objdump coreburn.exe```,

If you need that level of accuracy, you should probably look elsewhere anyway.

## Usage
Usage is extremely simple
- Windows: ```.\coreburn.exe [mode] coreID_1 coreID_2 ... coreID_n```
- Linux: ```./coreburn [mode] coreID_1 coreID_2 ... coreID_n```

Example: ```.\coreburn.exe -v128t 0 2 21``` means:
- Use the v128t (vector-128-bit) benchmark on logical cores 0, 2, and 21.

## Modes
Each mode uses a different benchmark:

- **INT:**  -i32l -i32t -i64l -i64t
- **FP:**   -f32l -f32t -f64l -f64t
- **VECT:** -v128l -v128t -v256l -v256t -v512l -v512t
- **MEM:**  -l1rl -l1rt -l2rl -l2rt -l3rl -l3rt -mrl -mrt

| mnemonic | meaning |
|----------|---------|
|ixxl      |integer xx-bit latency|
|ixxt      |integer xx-bit throughput|
|fxxl      |floating-point xx-bit latency|
|fxxt      |floating-point xx-bit throughput|
|vxxxl     |vector xxx-bit latency|
|vxxxt     |vector xxx-bit throughput|
|lxrl      |level-x cache read latency (random read)|
|lxrt      |level-x cache read throughput (sequential read)|
|mrl       |memory read latency (random read)|
|mrt       |memory read throughput (sequential read)|

## Interpreting the results
### Example case: instruction-level-parallelism in AVX2 (throughput)
I'll be using the following: ```.\coreburn.exe -v256t 0 2 21```

This particular benchmark is meant to test how fast your CPU is at running the following fragment:
```

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

```
Basically, a lot of multiply-adds without dependencies. If your CPU supports AVX2 and the compiler vectorizes this, you'll get a similar assembly:
```
	vpmulld	ymm9, ymm9, ymm1  # vect1-vect8 are stored in ymm2-ymm9
	vpmulld	ymm8, ymm8, ymm1  # ymm1 holds vecMul, ymm0 holds vecAdd
	vpmulld	ymm7, ymm7, ymm1
	vpmulld	ymm6, ymm6, ymm1
	vpmulld	ymm5, ymm5, ymm1  # vpmulld = vector packed multiply 32-bit int
	vpmulld	ymm4, ymm4, ymm1
	vpmulld	ymm3, ymm3, ymm1
	vpmulld	ymm2, ymm2, ymm1
	vpaddd	ymm9, ymm9, ymm0
	vpaddd	ymm8, ymm8, ymm0
	vpaddd	ymm7, ymm7, ymm0
	vpaddd	ymm6, ymm6, ymm0  # vpaddd = vector packed add 32-bit int
	vpaddd	ymm5, ymm5, ymm0
	vpaddd	ymm4, ymm4, ymm0
	vpaddd	ymm3, ymm3, ymm0
	vpaddd	ymm2, ymm2, ymm0
```
You're not iterating over arrays anymore, but running an instruction per array. 
The execution of this code is FAST, so we iterate it MANY times. You can see it in the output

As the code for each benchmark is different, it's not fair to compare results between them. 
When you run any benchmark, you'll get an output in this format:
```
.\coreburn.exe -v256t 0 2 21
Benching vect256 throughput with 10^9 iterations
Logical Core 0
                time: 2.705s

Logical Core 2
                time: 4.730s

Logical Core 21
                time: 7.083s
```
How can you interpret this? In my case, this was run on an intel meteor lake CPU, which uses an heterogeneous architecture 
with 6P-8E-2LPE cores (the P cores have hyperthreading). Core 0 is a P core, 2 is an E core, and 21 is LPE.

Take a look at this table:

|core|runtime|frequency*|Cycles Per Loop iteration (CPL** = (time*freq)/nºiterations)|
|----|-------|---------|-----------------------------------------------------------|
|P   |~2.7s  |5.1GHz   |(2.7s * 5.1GHz)/10^9iter = 13,77 CPL                       |
|E   |~4.7s  |3.8GHz   |(4.7s * 3.8GHz)/10^9iter = 17,86 CPL                       |
|LPE |~7.1s  |2.5GHz   |(7.1s * 2.5GHz)/10^9iter = 17,75 CPL                       |

_*max turbo frequencies from intel's documentation. Check your real frequencies when running the benchmark!!!_

_**Not to be confused with CPI (Cycles Per Instruction)_

Notice anything? The cycles per loop iteration for E and LPE cores are roughly the same, with the P core being significantly better.

This makes a LOT of sense, as **meteor lake's P cores are Redwood Cove, and its E/LPE cores are Crestmont**. Two different architectures with different purposes. 
It's easy to see here that, at least at the core level itself (not counting cache), **E and LPE cores are the same, but the former is clocked higher**.

project(coreburn C)

cmake_minimum_required(VERSION 3.5)

# set(CMAKE_C_FLAGS "-O2 -march=native -fopenmp")
# set(CMAKE_C_FLAGS "-O2 -fno-tree-vectorize") # FOR NO VECTORIZATION AT ALL, good for testing

add_executable( coreburn
	src/main.c
	src/int.c
	src/fp.c
	src/vect.c
	src/mem.c
)
 

find_package(OpenMP)
if(OpenMP_C_FOUND)
    target_link_libraries(coreburn PRIVATE OpenMP::OpenMP_C)
else()
    message(WARNING "OpenMP not found. Vectorization hints might be ignored.")
endif()

if(MSVC)
    target_compile_options(coreburn PRIVATE
        /O2          # Maximize speed
        /fp:fast     # for vectorizing floating point!
        /Oi          # enable intrinsics
    )
    
    # For x86_64, AVX2 is a safe bet.
    # For ARM64, NEON is default, so no flag is needed.
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
        target_compile_options(coreburn PRIVATE /arch:AVX2) # MSVC has no -march=native, shame 
endif()

else() # GCC / Clang
    target_compile_options(coreburn PRIVATE
        -O2
        -march=native
        -ffast-math
        -fno-strict-aliasing
    )
endif()
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

#set(CMAKE_C_COMPILER "nvcc")
set(PROJECT_NAME 2DEXHRMS)

# Status information
message("Using: ${CMAKE_C_COMPILER} with compiler ID ${CMAKE_C_COMPILER_ID} and compiler version ${CMAKE_C_COMPILER_VERSION}")

file(GLOB_RECURSE SOURCE_FILES "src/*.c")

include_directories(include/)

find_package(CUDA REQUIRED)
find_package(OpenMP)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif() 

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS})

list(APPEND CUDA_NVCC_FLAGS --ptxas-options= -Xptxas -dlcm=cg -use_fast_math -fopenmp)
list(APPEND CUDA_NVCC_FLAGS -g;)
list(APPEND CUDA_NVCC_FLAGS -Xptxas -O3 -std=c++11;)
list(APPEND CUDA_NVCC_FLAGS -lineinfo;)
list(APPEND CUDA_NVCC_FLAGS -Xcompiler;-O3;)
list(APPEND CUDA_NVCC_FLAGS -Xcompiler;-lm;)
list(APPEND CUDA_NVCC_FLAGS -Xcompiler;-fopenmp;)
list(APPEND CUDA_NVCC_FLAGS -Xcompiler;-Wall;)
list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_61,code=sm_61)
list(APPEND CMAKE_CXX_FLAGS "-Wall -O3 -fopenmp")
list(APPEND CMAKE_C_FLAGS "-O3 -fopenmp")


set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

message(STATUS "Using CXX compilation flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Using CUDA NVCC flags ${CUDA_NVCC_FLAGS}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} m)

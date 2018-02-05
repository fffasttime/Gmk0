# Introduction

  A simple wrapper of the neural network of leela-zero, which can also be applied in other AIs with similar network structure of AlphaGo Zero.

# Use

## Requirements

### Linux
* GCC, Clang
* boost 1.58.x or later (libboost-all-dev on Debian/Ubuntu)
* BLAS Library: OpenBLAS (libopenblas-dev) or (optionally) Intel MKL
* Standard OpenCL C headers (opencl-headers on Debian/Ubuntu, or at
https://github.com/KhronosGroup/OpenCL-Headers/tree/master/opencl22/)
* OpenCL ICD loader (ocl-icd-libopencl1 on Debian/Ubuntu, or reference implementation at https://github.com/KhronosGroup/OpenCL-ICD-Loader)

### Windows (You can get these packages by NuGet in vs2017)
* MSVC, any C++14 compiler
* boost 1.58.x or later
* OpenBLAS version=0.2.14.1
* opencl-nug version=0.777.12

### Hardware
An OpenCL capable device, preferably a very, very fast GPU, with recent
drivers is strongly recommended (OpenCL 1.2 support should be enough,
even OpenCL 1.1 might work). If you do not have a GPU, modify config.h in the
source and remove the line that says "#define USE_OPENCL". Then cpu implement
will be used.

## Examples
  In example.cpp there is a short example. Include the "nn_cpp.h" and provide a weight file in leelaz format. You can also set the parameters used in the network, or the default value will be used;
# Attention:
* The default size of Board is 7. You need to change the W and H to your size in OpenCL.cpp kernel sourceCode_convolve3.
* In config.h there are some essential parameters for the nerual network, adapt them according to your NN;
* This project only implements the forward function of the original complete NN. 

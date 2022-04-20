#pragma once
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cuda_runtime.h>
#include <cuda_profiler_api.h>

using std::min;
using std::max;
using std::abs;
using std::fprintf;
using std::exit;




// LAUNCH CONFIG
// -------------

// Limits
#define BLOCK_LIMIT 1024
#define GRID_LIMIT  65535

// For map-like operations
template <class T>
constexpr int BLOCK_DIM_M() noexcept { return 256; }
template <class T>
constexpr int GRID_DIM_M() noexcept  { return GRID_LIMIT; }

// For reduce-like operations
template <class T=float>
constexpr int BLOCK_DIM_RM() noexcept { return 256; }
template <class T=float>
constexpr int GRID_DIM_RM() noexcept  { return 1024; }
template <class T=float>
constexpr int BLOCK_DIM_RI() noexcept { return sizeof(T)<=4? 128:256; }
template <class T=float>
constexpr int GRID_DIM_RI() noexcept  { return 1024; }
template <class T=float>
constexpr int BLOCK_DIM_R() noexcept { return BLOCK_DIM_RM<T>(); }
template <class T=float>
constexpr int GRID_DIM_R() noexcept  { return GRID_DIM_RM<T>(); }




// TRY
// ---
// Log error if CUDA function call fails.

#ifndef TRY_CUDA
void tryCuda(cudaError err, const char* exp, const char* func, int line, const char* file) {
  if (err == cudaSuccess) return;
  fprintf(stderr,
    "%s: %s\n"
    "  in expression %s\n"
    "  at %s:%d in %s\n",
    cudaGetErrorName(err), cudaGetErrorString(err), exp, func, line, file);
  exit(err);
}

#define TRY_CUDA(exp) tryCuda(exp, #exp, __func__, __LINE__, __FILE__)
#endif

#ifndef TRY
#define TRY(exp) TRY_CUDA(exp)
#endif




// DEFINE
// ------
// Define thread, block variables.

#ifndef DEFINE_CUDA
#define DEFINE_CUDA(t, b, B, G) \
  const int t = threadIdx.x; \
  const int b = blockIdx.x; \
  const int B = blockDim.x; \
  const int G = gridDim.x;
#define DEFINE_CUDA2D(tx, ty, bx, by, BX, BY, GX, GY) \
  const int tx = threadIdx.x; \
  const int ty = threadIdx.y; \
  const int bx = blockIdx.x; \
  const int by = blockIdx.y; \
  const int BX = blockDim.x; \
  const int BY = blockDim.y; \
  const int GX = gridDim.x;  \
  const int GY = gridDim.y;
#endif

#ifndef DEFINE
#define DEFINE(t, b, B, G) \
  DEFINE_CUDA(t, b, B, G)
#define DEFINE2D(tx, ty, bx, by, BX, BY, GX, GY) \
  DEFINE_CUDA2D(tx, ty, bx, by, BX, BY, GX, GY)
#endif




// UNUSED
// ------
// Mark CUDA kernel variables as unused.

template <class T>
__device__ void unusedCuda(T&&) {}

#ifndef UNUSED_CUDA
#define UNUSED_CUDA(x) unusedCuda(x)
#endif

#ifndef UNUSED
#define UNUSED UNUSED_CUDA
#endif




// REMOVE IDE SQUIGGLES
// --------------------

#ifndef __global__
#define __global__
#define __host__
#define __device__
#define __shared__
void __syncthreads();
#endif




// REDUCE
// ------

template <class T=float>
int reduceMemcpySizeCu(size_t N) {
  const int B = BLOCK_DIM_RM<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_RM<T>());
  return G;
}

template <class T=float>
int reduceInplaceSizeCu(size_t N) {
  const int B = BLOCK_DIM_RI<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_RI<T>());
  return G;
}

template <class T=float>
int reduceSizeCu(size_t N) { return reduceMemcpySizeCu<T>(N); }




// SWAP
// ----

template <class T>
__device__ void swapCu(T& x, T& y) {
  T t = x; x = y; y = t;
}




// FILL
// ----

template <class T>
__device__ void fillKernelLoopU(T *a, size_t N, T v, size_t i, size_t DI) {
  for (; i<N; i+=DI)
    a[i] = v;
}


template <class T>
__global__ void fillKernelU(T *a, size_t N, T v) {
  DEFINE(t, b, B, G);
  fillKernelLoopU(a, N, v, B*b+t, G*B);
}


template <class T>
__host__ __device__ void fillCuU(T *a, size_t N, T v) {
  const int B = BLOCK_DIM_M<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_M<T>());
  fillKernelU<<<G, B>>>(a, N, v);
}




// FILL-AT
// -------

template <class T, class K>
__device__ void fillAtKernelLoopU(T *a, T v, const K *is, size_t IS, size_t i, size_t DI) {
  for (; i<IS; i+=DI)
    a[is[i]] = v;
}


template <class T, class K>
__global__ void fillAtKernelU(T *a, T v, const K *is, size_t IS) {
  DEFINE(t, b, B, G);
  fillAtKernelLoopU(a, v, is, IS, B*b+t, G*B);
}


template <class T, class K>
__host__ __device__ void fillAtCuU(T *a, T v, const K *is, size_t IS) {
  const int B = BLOCK_DIM_M<T>();
  const int G = min(int(ceilDiv(IS, size_t(B))), GRID_DIM_M<T>());
  fillAtKernelU<<<G, B>>>(a, v, is, IS);
}




// MULTIPLY
// --------

template <class T>
__device__ void multiplyKernelLoopW(T *a, const T *x, const T *y, size_t N, size_t i, size_t DI) {
  for (; i<N; i+=DI)
    a[i] = x[i] * y[i];
}


template <class T>
__global__ void multiplyKernelW(T *a, const T *x, const T* y, size_t N) {
  DEFINE(t, b, B, G);
  multiplyKernelLoopW(a, x, y, N, B*b+t, G*B);
}


template <class T>
void multiplyCuW(T *a, const T *x, const T* y, size_t N) {
  const int B = BLOCK_DIM_M<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_M<T>());
  multiplyKernelW<<<G, B>>>(a, x, y, N);
}




// MULTIPLY-VALUE
// --------------

template <class T>
__device__ void multiplyValueKernelLoopW(T *a, const T *x, T v, size_t N, size_t i, size_t DI) {
  for (; i<N; i+=DI)
    a[i] = x[i] * v;
}


template <class T>
__global__ void multiplyValueKernelW(T *a, const T *x, T v, size_t N) {
  DEFINE(t, b, B, G);
  multiplyValueKernelLoopW(a, x, v, N, B*b+t, G*B);
}


template <class T>
void multiplyValueCuW(T *a, const T *x, T v, size_t N) {
  const int B = BLOCK_DIM_M<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_M<T>());
  multiplyValueKernelW<<<G, B>>>(a, x, v, N);
}




// MAX
// ---

template <class T>
__device__ void maxKernelReduceW(T* a, size_t N, size_t i) {
  __syncthreads();
  for (N=N/2; N>0; N/=2) {
    if (i<N) a[i] = max(a[i], a[N+i]);
    __syncthreads();
  }
}


template <class T>
__device__ T maxKernelLoop(const T *x, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    a = max(a, x[i]);
  return a;
}


template <class T, int S=BLOCK_LIMIT>
__global__ void maxKernelW(T *a, const T *x, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = maxKernelLoop(x, N, B*b+t, G*B);
  maxKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T>
void maxMemcpyCuW(T *a, const T *x, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  maxKernelW<<<G, B>>>(a, x, N);
}

template <class T>
void maxInplaceCuW(T *a, const T *x, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  maxKernelW<<<G, B>>>(a, x, N);
  maxKernelW<<<1, G>>>(a, a, G);
}

template <class T>
void maxCuW(T *a, const T *x, size_t N) {
  maxMemcpyCuW(a, x, N);
}




// SUM
// ---

template <class T>
__device__ void sumKernelReduceW(T* a, size_t N, size_t i) {
  __syncthreads();
  for (N=N/2; N>0; N/=2) {
    if (i<N) a[i] += a[N+i];
    __syncthreads();
  }
}


template <class T>
__device__ T sumKernelLoop(const T *x, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    a += x[i];
  return a;
}


template <class T, int S=BLOCK_LIMIT>
__global__ void sumKernelW(T *a, const T *x, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = sumKernelLoop(x, N, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T>
void sumMemcpyCuW(T *a, const T *x, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumKernelW<<<G, B>>>(a, x, N);
}

template <class T>
void sumInplaceCuW(T *a, const T *x, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumKernelW<<<G, B>>>(a, x, N);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T>
void sumCuW(T *a, const T *x, size_t N) {
  sumMemcpyCuW(a, x, N);
}




// SUM-ABS
// -------

template <class T>
__device__ T sumAbsKernelLoop(const T *x, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    a += abs(x[i]);
  return a;
}


template <class T, int S=BLOCK_LIMIT>
__global__ void sumAbsKernelW(T *a, const T *x, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = sumAbsKernelLoop(x, N, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T>
void sumAbsMemcpyCuW(T *a, const T *x, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumAbsKernelW<<<G, B>>>(a, x, N);
}

template <class T>
void sumAbsInplaceCuW(T *a, const T *x, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumAbsKernelW<<<G, B>>>(a, x, N);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T>
void sumAbsCuW(T *a, const T *x, size_t N) {
  sumAbsMemcpyCuW(a, x, N);
}




// SUM-SQR
// -------

template <class T>
__device__ T sumSqrKernelLoop(const T *x, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    a += x[i]*x[i];
  return a;
}


template <class T, int S=BLOCK_LIMIT>
__global__ void sumSqrKernelW(T *a, const T *x, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = sumSqrKernelLoop(x, N, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T>
void sumSqrMemcpyCuW(T *a, const T *x, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumSqrKernelW<<<G, B>>>(a, x, N);
}

template <class T>
void sumSqrInplaceCuW(T *a, const T *x, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumSqrKernelW<<<G, B>>>(a, x, N);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T>
void sumSqrCuW(T *a, const T *x, size_t N) {
  sumSqrMemcpyCuW(a, x, N);
}




// SUM-AT
// ------

template <class T, class K>
__device__ T sumAtKernelLoop(const T *x, const K *is, size_t IS, size_t i, size_t DI) {
  T a = T();
  for (; i<IS; i+=DI)
    a += x[is[i]];
  return a;
}


template <class T, class K, int S=BLOCK_LIMIT>
__global__ void sumAtKernelW(T *a, const T *x, const K *is, size_t IS) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = sumAtKernelLoop(x, is, IS, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T, class K>
void sumAtMemcpyCuW(T *a, const T *x, const K *is, size_t IS) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(IS, size_t(B))), GRID_DIM_R<T>());
  sumAtKernelW<<<G, B>>>(a, x, is, IS);
}

template <class T, class K>
void sumAtInplaceCuW(T *a, const T *x, const K *is, size_t IS) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(IS, size_t(B))), GRID_DIM_R<T>());
  sumAtKernelW<<<G, B>>>(a, x, is, IS);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T, class K>
void sumAtCuW(T *a, const T *x, const K *is, size_t IS) {
  sumAtMemcpyCuW(a, x, is, IS);
}




// SUM-IF-NOT
// ----------

template <class T, class C>
__device__ T sumIfNotKernelLoop(const T *x, const C *cs, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    if (!cs[i]) a += x[i];
  return a;
}


template <class T, class C, int S=BLOCK_LIMIT>
__global__ void sumIfNotKernelW(T *a, const T *x, const C *cs, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = sumIfNotKernelLoop(x, cs, N, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T, class C>
void sumIfNotMemcpyCuW(T *a, const T *x, const C *cs, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumIfNotKernelW<<<G, B>>>(a, x, cs, N);
}

template <class T, class C>
void sumIfNotInplaceCuW(T *a, const T *x, const C *cs, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumIfNotKernelW<<<G, B>>>(a, x, cs, N);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T, class C>
void sumIfNotCuW(T *a, const T *x, const C *cs, size_t N) {
  sumIfNotMemcpyCuW(a, x, cs, N);
}




// SUM-MULTIPLY
// ------------

template <class T>
__device__ T sumMultiplyKernelLoop(const T *x, const T *y, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    a += x[i] * y[i];
  return a;
}


template <class T, int S=BLOCK_LIMIT>
__global__ void sumMultiplyKernelW(T *a, const T *x, const T *y, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = sumMultiplyKernelLoop(x, y, N, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T>
void sumMultiplyMemcpyCuW(T *a, const T *x, const T *y, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumMultiplyKernelW<<<G, B>>>(a, x, y, N);
}

template <class T>
void sumMultiplyInplaceCuW(T *a, const T *x, const T *y, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  sumMultiplyKernelW<<<G, B>>>(a, x, y, N);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T>
void sumMultiplyCuW(T *a, const T *x, const T *y, size_t N) {
  sumMultiplyMemcpyCuW(a, x, y, N);
}




// SUM-MULTIPLY-AT
// ---------------

template <class T, class K>
__device__ T sumMultiplyAtKernelLoop(const T *x, const T *y, const K *is, size_t IS, size_t i, size_t DI) {
  T a = T();
  for (; i<IS; i+=DI)
    a += x[is[i]] * y[is[i]];
  return a;
}


template <class T, class K, int S=BLOCK_LIMIT>
__global__ void sumMultiplyAtKernelW(T *a, const T *x, const T *y, const K *is, size_t IS) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = sumMultiplyAtKernelLoop(x, y, is, IS, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T, class K>
void sumMultiplyAtMemcpyCuW(T *a, const T *x, const T *y, const K *is, size_t IS) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(IS, size_t(B))), GRID_DIM_R<T>());
  sumMultiplyAtKernelW<<<G, B>>>(a, x, y, is, IS);
}

template <class T, class K>
void sumMultiplyAtInplaceCuW(T *a, const T *x, const T *y, const K *is, size_t IS) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(IS, size_t(B))), GRID_DIM_R<T>());
  sumMultiplyAtKernelW<<<G, B>>>(a, x, y, is, IS);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T, class K>
void sumMultiplyAtCuW(T *a, const T *x, const T *y, const K *is, size_t IS) {
  sumMultiplyAtMemcpyCuW(a, x, y, is, IS);
}




// L1-NORM
// -------

template <class T>
__device__ T l1NormKernelLoop(const T *x, const T *y, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    a += abs(x[i] - y[i]);
  return a;
}


template <class T, int S=BLOCK_LIMIT>
__global__ void l1NormKernelW(T *a, const T *x, const T *y, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = l1NormKernelLoop(x, y, N, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T>
void l1NormMemcpyCuW(T *a, const T *x, const T *y, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  l1NormKernelW<<<G, B>>>(a, x, y, N);
}

template <class T>
void l1NormInplaceCuW(T *a, const T *x, const T *y, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  l1NormKernelW<<<G, B>>>(a, x, y, N);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T>
void l1NormCuW(T *a, const T *x, const T *y, size_t N) {
  l1NormMemcpyCuW(a, x, y, N);
}




// L2-NORM
// -------
// Remember to sqrt the result!

template <class T>
__device__ T l2NormKernelLoop(const T *x, const T *y, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    a += (x[i] - y[i]) * (x[i] - y[i]);
  return a;
}


template <class T, int S=BLOCK_LIMIT>
__global__ void l2NormKernelW(T *a, const T *x, const T *y, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = l2NormKernelLoop(x, y, N, B*b+t, G*B);
  sumKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T>
void l2NormMemcpyCuW(T *a, const T *x, const T *y, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  l2NormKernelW<<<G, B>>>(a, x, y, N);
}

template <class T>
void l2NormInplaceCuW(T *a, const T *x, const T *y, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  l2NormKernelW<<<G, B>>>(a, x, y, N);
  sumKernelW<<<1, G>>>(a, a, G);
}

template <class T>
void l2NormCuW(T *a, const T *x, const T *y, size_t N) {
  l2NormMemcpyCuW(a, x, y, N);
}




// LI-NORM
// -------

template <class T>
__device__ T liNormKernelLoop(const T *x, const T *y, size_t N, size_t i, size_t DI) {
  T a = T();
  for (; i<N; i+=DI)
    a = max(a, abs(x[i] - y[i]));
  return a;
}


template <class T, int S=BLOCK_LIMIT>
__global__ void liNormKernelW(T *a, const T *x, const T *y, size_t N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[S];
  cache[t] = liNormKernelLoop(x, y, N, B*b+t, G*B);
  maxKernelReduceW(cache, B, t);
  if (t==0) a[b] = cache[0];
}


template <class T>
void liNormMemcpyCuW(T *a, const T *x, const T *y, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  liNormKernelW<<<G, B>>>(a, x, y, N);
}

template <class T>
void liNormInplaceCuW(T *a, const T *x, const T *y, size_t N) {
  const int B = BLOCK_DIM_R<T>();
  const int G = min(int(ceilDiv(N, size_t(B))), GRID_DIM_R<T>());
  liNormKernelW<<<G, B>>>(a, x, y, N);
  maxKernelW<<<1, G>>>(a, a, G);
}

template <class T>
void liNormCuW(T *a, const T *x, const T *y, int N) {
  liNormMemcpyCuW(a, x, y, N);
}

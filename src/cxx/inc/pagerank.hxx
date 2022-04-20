#pragma once
#include <vector>
#include <utility>
#include "_main.hxx"

using std::vector;
using std::move;




// LAUNCH CONFIG
// -------------

// For pagerank cuda block-per-vertex
template <class T=float>
constexpr int BLOCK_DIM_PRCB() noexcept { return 256; }
template <class T=float>
constexpr int GRID_DIM_PRCB()  noexcept { return GRID_LIMIT; }

// For pagerank cuda thread-per-vertex
template <class T=float>
constexpr int BLOCK_DIM_PRCT() noexcept { return 512; }
template <class T=float>
constexpr int GRID_DIM_PRCT()  noexcept { return GRID_LIMIT; }
template <class T=float>
constexpr int BLOCK_DIM_PRCT_LOWDENSITY() noexcept { return 512; }
template <class T=float>
constexpr int GRID_DIM_PRCT_LOWDENSITY()  noexcept { return 8192; }
template <class T=float>
constexpr int BLOCK_DIM_PRCT_HIGHDEGREE() noexcept { return 32; }
template <class T=float>
constexpr int GRID_DIM_PRCT_HIGHDEGREE()  noexcept { return 8192; }




// OTHER CONFIG
// ------------

// For pagerank cuda
template <class T=float>
constexpr int SWITCH_DEGREE_PRC() noexcept { return 64; }
template <class T=float>
constexpr int SWITCH_LIMIT_PRC()  noexcept { return 32; }

// For levelwise pagerank
template <class T=float>
constexpr int MIN_COMPUTE_PR()  noexcept { return 1000000; }  // 10
template <class T=float>
constexpr int MIN_COMPUTE_PRC() noexcept { return 1000000; }  // 5000000




// PAGERANK-OPTIONS
// ----------------

template <class T>
struct PagerankOptions {
  int  repeat;
  int  toleranceNorm;
  T    damping;
  T    tolerance;
  int  maxIterations;

  PagerankOptions(int repeat=1, int toleranceNorm=1, T damping=0.85, T tolerance=1e-6, int maxIterations=500) :
  repeat(repeat), toleranceNorm(toleranceNorm), damping(damping), tolerance(tolerance), maxIterations(maxIterations) {}
};




// PAGERANK-RESULT
// ---------------

template <class T>
struct PagerankResult {
  vector<T> ranks;
  int   iterations;
  float time;

  PagerankResult(vector<T>&& ranks, int iterations=0, float time=0) :
  ranks(ranks), iterations(iterations), time(time) {}

  PagerankResult(vector<T>& ranks, int iterations=0, float time=0) :
  ranks(move(ranks)), iterations(iterations), time(time) {}


  // Get initial ranks (when no vertices affected for dynamic pagerank).
  template <class G>
  static PagerankResult<T> initial(const G& x, const vector<T>* q=nullptr) {
    int  N = x.order();
    auto a = q? *q : createContainer(x, T());
    if (!q) fillValueAtU(a, x.vertexKeys(), T(1)/N);
    return {a, 0, 0};
  }
};

#pragma once
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "transpose.hxx"
#include "dynamic.hxx"
#include "pagerank.hxx"
#include "pagerankSeq.hxx"

using std::vector;
using std::swap;




// PAGERANK-LOOP
// -------------

template <class T>
int pagerankMonolithicSeqLoopU(vector<T>& a, vector<T>& r, vector<T>& c, const vector<T>& f, const vector<int>& vfrom, const vector<int>& efrom, const vector<int>& vdata, int i, int n, int N, T p, T E, int L, int EF) {
  int l = 0;
  while (l<L) {
    T c0 = pagerankTeleport(r, vdata, N, p);
    pagerankCalculateW(a, c, vfrom, efrom, i, n, c0);  // assume contribtions (c) is precalculated
    T el = pagerankError(a, r, i, n, EF); ++l;         // one iteration complete
    if (el<E || l>=L) break;                           // check tolerance, iteration limit
    multiplyValuesW(c, a, f, i, n);                    // update partial contributions (c)
    swap(a, r);                                        // final ranks always in (a)
  }
  return l;
}




// PAGERANK (STATIC / INCREMENTAL)
// -------------------------------

// Find pagerank using a single thread (pull, CSR).
// @param x  original graph
// @param xt transpose graph (with vertex-data=out-degree)
// @param q  initial ranks (optional)
// @param o  options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns {ranks, iterations, time}
template <class G, class H, class T=float>
PagerankResult<T> pagerankMonolithicSeq(const G& x, const H& xt, const vector<T> *q=nullptr, const PagerankOptions<T>& o={}) {
  int  N  = xt.order();  if (N==0) return PagerankResult<T>::initial(xt, q);
  auto ks = vertexKeys(xt);
  return pagerankSeq(xt, ks, 0, N, pagerankMonolithicSeqLoopU<T>, q, o);
}

template <class G, class T=float>
PagerankResult<T> pagerankMonolithicSeq(const G& x, const vector<T> *q=nullptr, const PagerankOptions<T>& o={}) {
  auto xt = transposeWithDegree(x);
  return pagerankMonolithicSeq(x, xt, q, o);
}




// PAGERANK (DYNAMIC)
// ------------------

template <class G, class H, class T=float>
PagerankResult<T> pagerankMonolithicSeqDynamic(const G& x, const H& xt, const G& y, const H& yt, const vector<T> *q=nullptr, const PagerankOptions<T>& o={}) {
  int  N = yt.order();                             if (N==0) return PagerankResult<T>::initial(yt, q);
  auto [ks, n] = dynamicInVertices(x, xt, y, yt);  if (n==0) return PagerankResult<T>::initial(yt, q);
  return pagerankSeq(yt, ks, 0, n, pagerankMonolithicSeqLoopU<T>, q, o);
}

template <class G, class T=float>
PagerankResult<T> pagerankMonolithicSeqDynamic(const G& x, const G& y, const vector<T> *q=nullptr, const PagerankOptions<T>& o={}) {
  auto xt = transposeWithDegree(x);
  auto yt = transposeWithDegree(y);
  return pagerankMonolithicSeqDynamic(x, xt, y, yt, q, o);
}

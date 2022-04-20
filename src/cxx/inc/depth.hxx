#pragma once
#include <tuple>
#include "dfs.hxx"
#include "deadEnds.hxx"

using std::make_tuple;




// DEPTH
// -----

template <class G, class K>
auto depth(const G& x, K u) {
  K a = 0; if (x.order()==0) return 0;
  dfsDepthDo(x, u, 0, [&](auto v, auto d) { if (d>a) a=d; });
  return ++a;
}




// MIN/MAX/AVG
// -----------

template <class G, class H>
auto minDepth(const G& x, const H& xt) {
  using K = typename G::key_type;
  K dmin = x.order(), D = 0;
  deadEndsForEach(xt, [&](auto u) {
    auto d = depth(x, u); ++D;
    if (d<dmin) dmin = d;
  });
  return D>0? dmin : 0;
}

template <class G, class H>
auto maxDepth(const G& x, const H& xt) {
  using K = typename G::key_type;
  K dmax = 0;
  deadEndsForEach(xt, [&](auto u) {
    auto d = depth(x, u);
    if (d>dmax) dmax = d;
  });
  return dmax;
}

template <class G, class H>
float avgDepth(const G& x, const H& xt) {
  using K = typename G::key_type;
  K ds = 0, D = 0;
  deadEndsForEach(xt, [&](auto u) {
    auto d = depth(x, u); ++D;
    ds += d;
  });
  return D>0? ds/float(D) : 0;
}


template <class G, class H>
auto minMaxAvgDepth(const G& x, const H& xt) {
  using K = typename G::key_type;
  K dmin = x.order();
  K dmax = 0, ds = 0, D = 0;
  deadEndsForEach(xt, [&](auto u) {
    auto d = depth(x, u); ++D;
    if (d<dmin) dmin = d;
    if (d>dmax) dmax = d;
    ds += d;
  });
  if (D==0) dmin = 0;
  float davg = D>0? ds/float(D) : 0;
  return make_tuple(dmin, dmax, davg);
}

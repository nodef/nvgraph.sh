#pragma once
#include <tuple>
#include "vertices.hxx"

using std::make_tuple;




// DEGREES
// -------

template <class G, class F>
auto degreesDo(const G& x, F fn) {
  using K = typename G::key_type;
  auto  a = createContainer(x, K());
  x.forEachVertexKey([&](auto u) { a[u] = x.degree(u); fn(u, x.degree(u)); });
  return a;
}
template <class G>
auto degrees(const G& x) {
  auto fn = [](auto u, auto d) {};
  return degreesDo(x, fn);
}




// MIN/MAX/AVG
// -----------

template <class G>
auto minDegree(const G& x) {
  using K = typename G::key_type;
  K dmin = x.order();
  x.forEachVertexKey([&](auto u) {
    auto d = x.degree(u);
    if (d<dmin) dmin = d;
  });
  return dmin;
}

template <class G>
auto maxDegree(const G& x) {
  using K = typename G::key_type;
  K dmax = 0;
  x.forEachVertexKey([&](auto u) {
    auto d = x.degree(u);
    if (d>dmax) dmax = d;
  });
  return dmax;
}

template <class G>
inline float avgDegree(const G& x) {
  size_t N = x.order();
  return N>0? x.size()/float(N) : 0;
}


template <class G>
auto minMaxAvgDegree(const G& x) {
  using K = typename G::key_type;
  K dmin = x.order();
  K dmax = 0;
  x.forEachVertexKey([&](auto u) {
    auto d = x.degree(u);
    if (d<dmin) dmin = d;
    if (d>dmax) dmax = d;
  });
  return make_tuple(dmin, dmax, avgDegree(x));
}

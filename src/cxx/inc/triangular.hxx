#pragma once
#include "update.hxx"




// LOWER-TRIANGULAR
// ----------------

template <class G, class H>
inline void lowerTriangularW(H& a, const G& x) {
  x.forEachVertex([&](auto u, auto d) { a.addVertex(u, d); });
  x.forEachVertexKey([&](auto u) {
    x.forEachEdge(u, [&](auto v, auto w) { if (u > v) a.addEdge(u, v, w); });
  });
  a.update();
}
template <class G>
inline auto lowerTriangular(const G& x) {
  G a; lowerTriangularW(a, x);
  return a;
}

#ifdef OPENMP
template <class G, class H>
inline void lowerTriangularOmpW(H& a, const G& x) {
  x.forEachVertex([&](auto u, auto d) { a.addVertex(u, d); });
  #pragma omp parallel
  {
    x.forEachVertexKey([&](auto u) {
      x.forEachEdge(u, [&](auto v, auto w) { if (u > v) addEdgeOmpU(a, u, v, w); });
    });
  }
  updateOmpU(a);
}
template <class G>
inline auto lowerTriangularOmp(const G& x) {
  G a; lowerTriangularOmpW(a, x);
  return a;
}
#endif




template <class G>
inline void lowerTriangularU(G& a) {
  a.forEachVertexKey([&](auto u) {
    a.forEachEdge(u, [&](auto v, auto w) { if (u <= v) a.removeEdge(u, v); });
  });
}

#ifdef OPENMP
template <class G>
inline void lowerTriangularOmpU(G& a) {
  #pragma omp parallel
  {
    a.forEachVertexKey([&](auto u) {
      a.forEachEdge(u, [&](auto v, auto w) { if (u <= v) removeEdgeOmpU(a, u, v); });
    });
  }
  updateOmpU(a);
}
#endif

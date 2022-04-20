#pragma once




// LOWER-TRIANGULAR
// ----------------

template <class G, class H>
void lowerTriangularW(H& a, const G& x) {
  x.forEachVertex([&](auto u, auto d) { a.addVertex(u, d); });
  x.forEachVertexKey([&](auto u) {
    x.forEachEdge(u, [&](auto v, auto w) { if (u > v) a.addEdge(u, v, w); });
  });
}

template <class G>
auto lowerTriangular(const G& x) {
  G a; lowerTriangularW(a, x);
  return a;
}

template <class G>
void lowerTriangularU(G& a) {
  a.forEachVertexKey([&](auto u) {
    a.forEachEdge(u, [&](auto v, auto w) { if (u <= v) a.removeEdge(u, v); });
  });
}

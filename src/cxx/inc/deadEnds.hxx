#pragma once
#include <vector>
#include <unordered_set>

using std::vector;
using std::unordered_set;




// IS DEAD-END
// -----------

template <class G, class K>
inline bool isDeadEnd(const G& x, K u) {
  return x.degree(u) == 0;
}




// DEAD-ENDS
// ---------

template <class G, class F>
inline void deadEndsForEach(const G& x, F fn) {
  x.forEachVertexKey([&](auto u) { if (isDeadEnd(x, u)) fn(u); });
}
template <class G>
inline auto deadEnds(const G& x) {
  using K = typename G::key_type; vector<K> a;
  deadEndsForEach(x, [&](auto u) { a.push_back(u); });
  return a;
}
template <class G>
inline auto deadEndCount(const G& x) {
  using K = typename G::key_type; K a = 0;
  deadEndsForEach(x, [&](auto u) { ++a; });
  return a;
}

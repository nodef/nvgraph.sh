#pragma once
#include <numeric>
#include <algorithm>
#include <vector>
#include "_main.hxx"

using std::vector;
using std::iota;
using std::equal;
using std::transform;




// SOURCE OFFSETS
// --------------

template <class T, class G, class KS>
inline auto sourceOffsetsAs(const G& x, const KS& ks) {
  vector<T> a; T i = T();
  a.reserve(x.order()+1);
  for (auto u : ks) {
    a.push_back(i);
    i += T(x.degree(u));
  }
  a.push_back(i);
  return a;
}
template <class G, class KS>
inline auto sourceOffsets(const G& x, const KS& ks) {
  return sourceOffsetsAs<size_t>(x, ks);
}
template <class G>
inline auto sourceOffsets(const G& x) {
  return sourceOffsets(x, x.vertexKeys());
}




// DESTINATION INDICES
// -------------------

template <class T, class G, class KS>
inline auto destinationIndicesAs(const G& x, const KS& ks) {
  auto ids = valueIndexUnorderedMap(ks); vector<T> a;
  for (auto u : ks)
    x.forEachEdgeKey(u, [&](auto v) { a.push_back(T(ids[v])); });
  return a;
}
template <class G, class KS>
inline auto destinationIndices(const G& x, const KS& ks) {
  using  K = typename G::key_type;
  return destinationIndicesAs<K>(x, ks);
}
template <class G>
inline auto destinationIndices(const G& x) {
  return destinationIndices(x, x.vertexKeys());
}

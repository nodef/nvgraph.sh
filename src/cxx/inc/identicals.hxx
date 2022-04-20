#pragma once
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include "_main.hxx"
#include "vertices.hxx"

using std::map;
using std::vector;
using std::move;
using std::sort;




template <class G, class J>
auto edgeIdenticalsFromSize(const G& x, const J& ks, size_t n) {
  using K = typename G::key_type;
  using vec = vector<K>;
  map<vec, vec> m; vec es;
  // Find groups of identicals.
  for (auto u : ks) {
    copyWrite(x.edgeKeys(u), es);
    sortValues(es);
    m[es].push_back(u);
  }
  // Copy identicals from given size in sorted order.
  vector2d<K> a;
  for (auto& p : m) {
    auto& is = p.second;
    if (is.size()<n) continue;
    sortValues(is);
    a.push_back(move(is));
  }
  return a;
}

template <class G>
auto edgeIdenticalsFromSize(const G& x, size_t n) {
  return edgeIdenticalsFromSize(x, x.vertexKeys(), n);
}

template <class G, class J>
auto edgeIdenticals(const G& x, const J& ks) {
  return edgeIdenticalsFromSize(x, ks, 2);
}

template <class G>
auto edgeIdenticals(const G& x) {
  return edgeIdenticals(x, x.vertexKeys());
}

#pragma once
#include <vector>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"

using std::vector;




template <class G, class H, class K>
K chainTraverse(vector<K>& a, vector<bool>& vis, const G& x, const H& xt, K v) {
  while (!vis[v] && x.degree(v)==1 && xt.degree(v)==1) {
    vis[v] = true;
    a.push_back(v);
    v = edge(x, v);
  }
  return v;
}


template <class G, class H, class J>
auto chainsFromSize(const G& x, const H& xt, const J& ks, size_t n) {
  using K = typename G::key_type;
  auto vis = createContainer(x, bool());
  vector2d<K> a; vector<K> b;
  for (auto v : ks) {
    b.clear();
    // Traverse back.
    auto u = chainTraverse(b, vis, xt, x, v);
    if (x.degree(u)==1) b.push_back(u);
    reverseValues(b);
    if (b.size()>0) b.pop_back();
    // Traverse front.
    auto w = chainTraverse(b, vis, x, xt, v);
    if (xt.degree(w)==1) b.push_back(w);
    // Record chain.
    if (b.size()<n) continue;
    a.push_back(b);
  }
  return a;
}
template <class G, class H>
inline auto chainsFromSize(const G& x, const H& xt, size_t n) {
  return chainsFromSize(x, xt, x.vertexKeys(), n);
}
template <class G, class H, class J>
inline auto chains(const G& x, const H& xt, const J& ks) {
  return chainsFromSize(x, xt, ks, 2);
}
template <class G, class H>
inline auto chains(const G& x, const H& xt) {
  return chains(x, xt, x.vertexKeys());
}

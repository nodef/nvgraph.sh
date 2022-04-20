#pragma once
#include <vector>
#include "_main.hxx"
#include "vertices.hxx"
#include "dfs.hxx"

using std::vector;



// COMPONENTS
// ----------
// Finds Strongly Connected Components (SCC) using Kosaraju's algorithm.

template <class G, class H>
auto components(const G& x, const H& xt) {
  using K = typename G::key_type;
  vector2d<K> a; vector<K> vs;
  // original dfs
  auto vis = createContainer(x, bool());
  x.forEachVertexKey([&](auto u) {
    if (!vis[u]) dfsEndW(vs, vis, x, u);
  });
  // transpose dfs
  fillValue(vis, false);
  while (!vs.empty()) {
    auto u = vs.back(); vs.pop_back();
    if (vis[u]) continue;
    a.push_back(vector<K>());
    dfsW(a.back(), vis, xt, u);
  }
  return a;
}




// COMPONENTS-IDS
// --------------
// Get component id of each vertex.

template <class G, class K>
auto componentIds(const G& x, const vector2d<K>& cs) {
  auto a = createContainer(x, K()); K i = 0;
  for (const auto& c : cs) {
    for (K u : c)
      a[u] = i;
    i++;
  }
  return a;
}




// BLOCKGRAPH
// ----------
// Each component is represented as a vertex.

template <class H, class G, class K>
void blockgraphW(H& a, const G& x, const vector2d<K>& cs) {
  auto c = componentIds(x, cs);
  x.forEachVertexKey([&](auto u) {
    a.addVertex(c[u]);
    x.forEachEdgeKey(u, [&](auto v) {
      if (c[u] != c[v]) a.addEdge(c[u], c[v]);
    });
  });
  a.correct();
}
template <class G, class K>
inline auto blockgraph(const G& x, const vector2d<K>& cs) {
  G a; blockgraphW(a, x, cs);
  return a;
}




// COMPONENTS-EQUAL
// ----------------

template <class G, class K>
bool componentsEqual(const G& x, const vector<K>& xc, const G& y, const vector<K>& yc) {
  if (xc != yc) return false;
  for (size_t i=0, I=xc.size(); i<I; i++)
    if (!verticesEqual(x, xc[i], y, yc[i])) return false;
  return true;
}
template <class G, class H, class K>
inline bool componentsEqual(const G& x, const H& xt, const vector<K>& xc, const G& y, const H& yt, const vector<K>& yc) {
  return componentsEqual(x, xc, y, yc) && componentsEqual(xt, xc, yt, yc);
}




// COMPONENTS-HASH
// ---------------

template <class K>
auto componentsHash(const vector2d<K>& cs) {
  vector<size_t> a;
  for (const auto& c : cs)
    a.push_back(hashValue(c));
  return a;
}

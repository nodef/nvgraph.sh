#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "degree.hxx"
#include "dfs.hxx"
#include "components.hxx"

using std::vector;
using std::unordered_map;
using std::reverse;
using std::swap;




// TOPOLOGICAL-SORT
// ----------------
// Arrrange vertices in dependency order.
// Top level vertices may not always come first.

template <class G>
auto topologicalSort(const G& x) {
  using K = typename G::key_type; vector<K> a;
  auto vis = createContainer(x, bool());
  x.forEachVertexKey([&](auto u) {
    if (!vis[u]) dfsEndW(a, vis, x, u);
  });
  reverse(a.begin(), a.end());
  return a;
}




// LEVELWISE-SORT
// --------------
// Arrange vertices in dependency and level order.
// Top level vertices always come first.

template <class G, class K, class F>
void levelwiseFrontierDo(const G& x, const vector<K>& frnt, vector<K>& ideg, F fn) {
  for (auto u : frnt)
    x.forEachEdgeKey(u, [&](auto v) { if (--ideg[v] == 0) fn(v); });
}
template <class G, class H, class F>
void levelwiseFrontiersDo(const G& x, const H& xt, F fn) {
  using  K = typename G::key_type;
  vector<K> frnt, frnu, a;
  auto ideg = degreesDo(xt, [&](auto u, auto d) {
    if (d == 0) frnt.push_back(u);
  });
  while (!frnt.empty()) {
    fn(frnt);
    frnu.clear();
    levelwiseFrontierDo(x, frnt, ideg, [&](auto v) { frnu.push_back(v); });
    swap(frnu, frnt);
  }
}
template <class G, class H>
inline auto levelwiseSort(const G& x, const H& xt) {
  using K = typename G::key_type; vector<K> a;
  levelwiseFrontiersDo(x, xt, [&](const auto& frnt) { copyAppend(frnt, a); });
  return a;
}




// LEVELWISE-GROUPS
// ----------------
// Arrange groups of vertices in dependency and level order.
// Vertices belonging to the same level come in a group.

template <class G, class H>
inline auto levelwiseGroups(const G& x, const H& xt) {
  using K = typename G::key_type; vector2d<K> a;
  levelwiseFrontiersDo(x, xt, [&](const auto& frnt) { a.push_back(frnt); });
  return a;
}
template <class G, class H>
inline auto levelwiseGroupIndices(const G& x, const H& xt) {
  using K = typename G::key_type;
  vector<K> a(x.span()); K i = 0;
  levelwiseFrontiersDo(x, xt, [&](const auto& frnt) { fillValueAt(a, frnt, i++); });
  return a;
}




// TOPOLOGICAL-COMPONENTS
// ----------------------
// Get components in topological order.

template <class G, class K>
inline void topologicalComponentsW(vector2d<K>& cs, const G& b) {
  auto bks = topologicalSort(b);
  reorderDirtyU(cs, bks);
}
template <class G, class K>
inline auto topologicalComponentsFrom(const vector2d<K>& cs, const G& b) {
  auto bks = topologicalSort(b);
  return copyAtVector(cs, bks);
}
template <class G, class H>
inline auto topologicalComponents(const G& x, const H& xt) {
  auto cs = components(x, xt);
  auto b  = blockgraph(x, cs);
  topologicalComponentsW(cs, b);
  return cs;
}




// LEVELWISE-COMPONENTS
// --------------------

template <class G, class H, class K>
inline void levelwiseComponentsW(vector2d<K>& cs, const G& b, const H& bt) {
  auto bks = levelwiseSort(b, bt);
  reorderDirtyU(cs, bks);
}
template <class G, class H, class K>
inline auto levelwiseComponentsFrom(const vector2d<K>& cs, const G& b, const H& bt) {
  auto bks = levelwiseSort(b, bt);
  return copyAtVector(cs, bks);
}
template <class G, class H>
inline auto levelwiseComponents(const G& x, const H& xt) {
  auto cs = components(x, xt);
  auto b  = blockgraph(x, cs);
  auto bt = transpose(x);
  levelwiseComponentsW(cs, b, bt);
  return cs;
}




// LEVELWISE-GROUPED-COMPONENTS
// ----------------------------

template <class G, class H, class K>
auto levelwiseGroupedComponentsFrom(const vector2d<K>& cs, const G& b, const H& bt) {
  vector2d<K> a;
  auto bgs = levelwiseGroups(b, bt);
  for (const auto& g : bgs)
    a.push_back(joinAtVector(cs, g));
  return a;
}
template <class G, class H>
inline auto levelwiseGroupedComponents(const G& x, const H& xt) {
  auto cs = components(x, xt);
  auto b  = blockgraph(x, cs);
  auto bt = transpose(b);
  return levelwiseGroupedComponentsFrom(cs, b, bt);
}

#pragma once
#include <utility>
#include <iterator>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include "_main.hxx"
#include "vertices.hxx"
#include "components.hxx"

using std::iterator_traits;
using std::vector;
using std::unordered_set;
using std::make_pair;
using std::max;




// ADJUST-RANKS
// ------------
// For calculating inital ranks for incremental/dynamic pagerank.

template <class T, class J>
void adjustRanks(vector<T>& a, const vector<T>& r, const J& Kx, const J& Ky, T radd, T rmul, T rset) {
  using I = decltype(Kx.begin());
  using K = typename iterator_traits<I>::value_type;
  unordered_set<K> Sx(Kx.begin(), Kx.end());
  for (auto u : Ky)
    a[u] = Sx.count(u)==0? rset : (r[u]+radd)*rmul;  // vertex new/old
}
template <class T, class J>
auto adjustRanks(size_t N, const vector<T>& r, const J& ks0, const J& ks1, T radd, T rmul, T rset) {
  vector<T> a(N); adjustRanks(a, r, ks0, ks1, radd, rmul, rset);
  return a;
}




// CHANGED-VERTICES
// ----------------
// Find vertices with edges added/removed.

template <class G, class K>
bool isChangedVertex(const G& x, const G& y, K u) {
  return !x.hasVertex(u) || !verticesEqual(x, u, y, u);
}
template <class G, class H, class K>
bool isChangedVertex(const G& x, const H& xt, const G& y, const H& yt, K u) {
  return !x.hasVertex(u) || !verticesEqual(x, xt, u, y, yt, u);  // both ways
}

template <class G, class F>
void changedVerticesDo(const G& x, const G& y, F fn) {
  for (auto u : y.vertexKeys())
    if (isChangedVertex(x, y, u)) fn(u);
}
template <class G, class H, class F>
void changedVerticesDo(const G& x, const H& xt, const G& y, const H& yt, F fn) {
  for (auto u : y.vertexKeys())
    if (isChangedVertex(x, xt, y, yt, u)) fn(u);
}

template <class G>
auto changedVertices(const G& x, const G& y) {
  using K = typename G::key_type; vector<K> a;
  changedVerticesDo(x, y, [&](auto u) { a.push_back(u); });
  return a;
}
template <class G, class H>
auto changedVertices(const G& x, const H& xt, const G& y, const H& yt) {
  using K = typename G::key_type; vector<K> a;
  changedVerticesDo(x, xt, y, yt, [&](auto u) { a.push_back(u); });
  return a;
}




// AFFECTED-VERTICES
// -----------------
// Find vertices reachable from changed vertices.

template <class G>
bool hasAffectedDeadEnd(const G& x, const G& y, const vector<bool>& vis) {
  for (auto u : x.vertexKeys())
    if (isDeadEnd(x, u) && !y.hasVertex(u)) return true;
  for (auto u : y.vertexKeys())
    if (isDeadEnd(y, u) && vis[u]) return true;
  return false;
}


template <class G, class H>
bool affectedVerticesMark(vector<bool>& vis, const G& x, const H& xt, const G& y, const H& yt) {
  changedVerticesDo(x, xt, y, yt, [&](auto u) { dfsMarkLoop(vis, y, u); });
  return hasAffectedDeadEnd(x, y, vis);
}
template <class G, class H>
bool affectedInVerticesMark(vector<bool>& vis, const G& x, const H& xt, const G& y, const H& yt) {
  changedVerticesDo(xt, yt, [&](auto u) { dfsMarkLoop(vis, y, u); });
  return hasAffectedDeadEnd(x, y, vis);
}
template <class G>
bool affectedOutVerticesMark(vector<bool>& vis, const G& x, const G& y) {
  changedVerticesDo(x, y, [&](auto u) { dfsMarkLoop(vis, y, u); });
  return hasAffectedDeadEnd(x, y, vis);
}


template <class G, class F>
void affectedVerticesDoInt(const G& x, const G& y, const vector<bool>& vis, F fn) {
  if (hasAffectedDeadEnd(x, y, vis)) forEach(y.vertexKeys(), fn);
  else forEach(y.vertexKeys(), [&](auto u) { if (vis[u]) fn(u); });
}
template <class G, class H, class F>
void affectedVerticesDo(const G& x, const H& xt, const G& y, const H& yt, F fn) {
  auto vis = createContainer(y, bool());
  changedVerticesDo(x, xt, y, yt, [&](auto u) { dfsMarkLoop(vis, y, u); });
  affectedVerticesDoInt(x, y, vis, fn);
}
template <class G, class H, class F>
void affectedInVerticesDo(const G& x, const H& xt, const G& y, const H& yt, F fn) {
  auto vis = createContainer(y, bool());
  changedVerticesDo(xt, yt, [&](auto u) { dfsMarkLoop(vis, y, u); });
  affectedVerticesDoInt(x, y, vis, fn);
}
template <class G, class H, class F>
void affectedOutVerticesDo(const G& x, const G& y, F fn) {
  auto vis = createContainer(y, bool());
  changedVerticesDo(x, y, [&](auto u) { dfsMarkLoop(vis, y, u); });
  affectedVerticesDoInt(x, y, vis, fn);
}


template <class G, class H>
auto affectedVertices(const G& x, const H& xt, const G& y, const H& yt) {
  using K = typename G::key_type; vector<K> a;
  affectedVerticesDo(x, xt, y, yt, [&](auto u) { a.push_back(u); });
  return a;
}
template <class G, class H>
auto affectedInVertices(const G& x, const H& xt, const G& y, const H& yt) {
  using K = typename G::key_type; vector<K> a;
  affectedInVerticesDo(x, xt, y, yt, [&](auto u) { a.push_back(u); });
  return a;
}
template <class G>
auto affectedOutVertices(const G& x, const G& y) {
  using K = typename G::key_type; vector<K> a;
  affectedOutVerticesDo(x, y, [&](auto u) { a.push_back(u); });
  return a;
}




// DYNAMIC-VERTICES
// ----------------
// Find affected, unaffected vertices (vertices, no. affected).

template <class G, class FA>
auto dynamicVerticesByMark(const G& y, FA fa) {
  using K = typename G::key_type;
  auto vis = createContainer(y, bool());
  if(fa(vis)) return make_pair(vertexKeys(y), size_t(y.order()));
  vector<K> a; size_t n = 0;
  for (auto u : y.vertexKeys())
    if (vis[u]) { a.push_back(u); ++n; }
  return make_pair(a, n);
}

template <class G, class FA>
auto dynamicVerticesBy(const G& y, FA fa) {
  using K = typename G::key_type;
  vector<K> a; unordered_set<K> aff;
  fa([&](auto u) { a.push_back(u); aff.insert(u); });
  for (auto u : y.vertexKeys())
    if (aff.count(u)==0) a.push_back(u);
  return make_pair(a, aff.size());
}


template <class G, class H>
auto dynamicVertices(const G& x, const H& xt, const G& y, const H& yt) {
  return dynamicVerticesByMark(y, [&](auto& vis) { return affectedVerticesMark(vis, x, xt, y, yt); });
}
template <class G, class H>
auto dynamicInVertices(const G& x, const H& xt, const G& y, const H& yt) {
  return dynamicVerticesByMark(y, [&](auto& vis) { return affectedInVerticesMark(vis, x, xt, y, yt); });
}
template <class G>
auto dynamicOutVertices(const G& x, const G& y) {
  return dynamicVerticesByMark(y, [&](auto& vis) { return affectedOutVerticesMark(vis, x, y); });
}




// CHANGED-COMPONENTS
// ------------------
// Find components with edges added/removed.

template <class G, class K, class F>
void changedComponentIndicesDo(const G& x, const G& y, const vector2d<K>& cs, F fn) {
  for (K i=0; i<cs.size(); ++i)
    if (!componentsEqual(x, cs[i], y, cs[i])) fn(i);
}
template <class G, class H, class K, class F>
void changedComponentIndicesDo(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, F fn) {
  for (K i=0; i<cs.size(); ++i)
    if (!componentsEqual(x, xt, cs[i], y, yt, cs[i])) fn(i);  // both ways
}
template <class G, class H, class K, class F>
void changedInComponentIndicesDo(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, F fn) {
  return changedComponentIndicesDo(xt, yt, cs, fn);
}
template <class G, class H, class K, class F>
void changedOutComponentIndicesDo(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, F fn) {
  return changedComponentIndicesDo(x, y, cs, fn);
}


template <class G, class K>
auto changedComponentIndices(const G& x, const G& y, const vector2d<K>& cs) {
  vector<K> a; changedComponentIndicesDo(x, y, cs, [&](auto i) { a.push_back(i); });
  return a;
}
template <class G, class H, class K>
auto changedComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs) {
  vector<K> a; changedVerticesDo(x, xt, y, yt, cs, [&](auto i) { a.push_back(i); });
  return a;
}
template <class G, class H, class K>
auto changedInComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs) {
  return changedComponentIndices(xt, yt, cs);
}
template <class G, class H, class K>
auto changedOutComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs) {
  return changedComponentIndices(x, y, cs);
}




// AFFECTED-COMPONENTS
// -------------------
// Find components reachable from changed components.

template <class G, class K>
bool hasAffectedDeadEnd(const G& x, const G& y, const vector2d<K>& cs, const vector<bool>& vis) {
  for (auto u : x.vertexKeys())
    if (isDeadEnd(x, u) && !y.hasVertex(u)) return true;
  for (K i=0; i<cs.size(); ++i) {
    if (!vis[i]) continue;
    for (auto u : cs[i])
      if (isDeadEnd(y, u)) return true;
  }
  return false;
}


template <class G, class H, class K, class B>
bool affectedComponentIndicesMark(vector<bool>& vis, const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b) {
  changedComponentIndicesDo(x, xt, y, yt, cs, [&](auto i) { dfsMarkLoop(vis, b, i); });
  return hasAffectedDeadEnd(x, y, cs, vis);
}
template <class G, class H, class K, class B>
bool affectedInComponentIndicesMark(vector<bool>& vis, const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b) {
  changedComponentIndicesDo(xt, yt, cs, [&](auto i) { dfsMarkLoop(vis, b, i); });
  return hasAffectedDeadEnd(x, y, cs, vis);
}
template <class G, class K, class B>
bool affectedOutComponentIndicesMark(vector<bool>& vis, const G& x, const G& y, const vector2d<K>& cs, const B& b) {
  changedComponentIndicesDo(x, y, cs, [&](auto i) { dfsMarkLoop(vis, b, i); });
  return hasAffectedDeadEnd(x, y, cs, vis);
}


template <class G, class K, class F>
void affectedComponentIndicesDoInt(const G& x, const G& y, const vector2d<K>& cs, const vector<bool>& vis, F fn) {
  if (hasAffectedDeadEnd(x, y, cs, vis)) { for (K i=0; i<cs.size(); ++i) fn(i); }
  else { for (K i=0; i<cs.size(); ++i) { if (vis[i]) fn(i); } }
}
template <class G, class H, class K, class B, class F>
void affectedComponentIndicesDo(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b, F fn) {
  auto vis = createContainer(b, bool());
  changedComponentIndicesDo(x, xt, y, yt, cs, [&](auto i) { dfsMarkLoop(vis, b, i); });
  affectedComponentIndicesDoInt(x, y, cs, vis, fn);
}
template <class G, class H, class K, class B, class F>
void affectedInComponentIndicesDo(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b, F fn) {
  auto vis = createContainer(b, bool());
  changedInComponentIndicesDo(x, xt, y, yt, cs, [&](auto i) { dfsMarkLoop(vis, b, i); });
  affectedComponentIndicesDoInt(x, y, cs, vis, fn);
}
template <class G, class H, class K, class B, class F>
void affectedOutComponentIndicesDo(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b, F fn) {
  auto vis = createContainer(b, bool());
  changedOutComponentIndicesDo(x, xt, y, yt, cs, [&](auto i) { dfsMarkLoop(vis, b, i); });
  affectedComponentIndicesDoInt(x, y, cs, vis, fn);
}


template <class G, class H, class K, class B>
auto affectedComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b) {
  vector<K> a; affectedComponentIndicesForEach(x, xt, y, yt, cs, b, [&](auto i) { a.push_back(i); });
  return a;
}
template <class G, class H, class K, class B>
auto affectedInComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b) {
  vector<K> a; affectedInComponentIndicesForEach(x, xt, y, yt, cs, b, [&](auto i) { a.push_back(i); });
  return a;
}
template <class G, class H, class K, class B>
auto affectedOutComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b) {
  vector<K> a; affectedOutComponentIndicesForEach(x, xt, y, yt, cs, b, [&](auto i) { a.push_back(i); });
  return a;
}




// DYNAMIC-COMPONENTS
// ------------------
// Find affected, unaffected components (components, no. affected).

template <class G, class K, class FA>
auto dynamicComponentIndicesByMark(const G& y, const vector2d<K>& cs, FA fa) {
  auto vis = createContainer(y, bool());
  if(fa(vis)) return make_pair(rangeVector(K(cs.size())), cs.size());
  vector<K> a; size_t n = 0;
  for (K i=0; i<cs.size(); ++i)
    if (vis[i]) { a.push_back(i); ++n; }
  return make_pair(a, n);
}

template <class G, class K, class FA>
auto dynamicComponentIndicesBy(const G& y, const vector2d<K>& cs, FA fa) {
  vector<K> a; unordered_set<K> aff;
  fa([&](auto i) { a.push_back(i); aff.insert(i); });
  for (K i=0, I=cs.size(); i<I; ++i)
    if (aff.count(i)==0) a.push_back(i);
  return make_pair(a, aff.size());
}


template <class G, class H, class K, class B>
auto dynamicComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b) {
  return dynamicComponentIndicesByMark(y, cs, [&](auto& vis) {
    return affectedComponentIndicesMark(vis, x, xt, y, yt, cs, b);
  });
}
template <class G, class H, class K, class B>
auto dynamicInComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b) {
  return dynamicComponentIndicesByMark(y, cs, [&](auto& vis) {
    return affectedInComponentIndicesMark(vis, x, xt, y, yt, cs, b);
  });
}
template <class G, class H, class K, class B>
auto dynamicOutComponentIndices(const G& x, const H& xt, const G& y, const H& yt, const vector2d<K>& cs, const B& b) {
  return dynamicComponentIndicesByMark(y, cs, [&](auto& vis) {
    return affectedOutComponentIndicesDo(vis, x, xt, y, yt, cs, b);
  });
}

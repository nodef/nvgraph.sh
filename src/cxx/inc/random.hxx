#pragma once
#include <random>
#include <vector>

using std::uniform_real_distribution;




// ADD-RANDOM-EDGE
// ---------------

template <class G, class K, class R>
void addRandomEdge(G& a, R& rnd, K span) {
  uniform_real_distribution<> dis(0.0, 1.0);
  K u = K(dis(rnd) * span);
  K v = K(dis(rnd) * span);
  a.addEdge(u, v);
}


template <class G, class K, class R>
void addRandomEdgeByDegree(G& a, R& rnd, K span) {
  uniform_real_distribution<> dis(0.0, 1.0);
  double deg = a.size() / a.span();
  K un = K(dis(rnd) * deg * span);
  K vn = K(dis(rnd) * deg * span);
  K u = -1, v = -1, n = 0;
  for (K w : a.vertexKeys()) {
    if (un<0 && un > n+a.degree(w)) u = w;
    if (vn<0 && vn > n+a.degree(w)) v = w;
    if (un>0 && vn>=0) break;
    n += a.degree(w);
  }
  if (u<0) u = K(un/deg);
  if (v<0) v = K(vn/deg);
  a.addEdge(u, v);
}




// REMOVE-RANDOM-EDGE
// ------------------

template <class G, class K, class R>
bool removeRandomEdge(G& a, R& rnd, K u) {
  uniform_real_distribution<> dis(0.0, 1.0);
  if (a.degree(u) == 0) return false;
  K vi = K(dis(rnd) * a.degree(u)), i = 0;
  for (K v : a.edgesKeys(u))
    if (i++ == vi) { a.removeEdge(u, v); return true; }
  return false;
}


template <class G, class R>
bool removeRandomEdge(G& a, R& rnd) {
  using K = typename G::key_type;
  uniform_real_distribution<> dis(0.0, 1.0);
  K u = K(dis(rnd) * a.span());
  return removeRandomEdge(a, rnd, u);
}


template <class G, class R>
bool removeRandomEdgeByDegree(G& a, R& rnd) {
  using K = typename G::key_type;
  uniform_real_distribution<> dis(0.0, 1.0);
  K v = K(dis(rnd) * a.size()), n = 0;
  for (K u : a.vertexKeys()) {
    if (v > n+a.degree(u)) n += a.degree(u);
    else return removeRandomEdge(a, rnd, u);
  }
  return false;
}

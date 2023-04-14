#pragma once
#include <vector>
#include <utility>

using std::vector;
using std::swap;




// BFS
// ---
// Breadth First Search (BFS) graph traversal.

/**
 * Find vertices visited with BFS.
 * @param vis vertex visited? (updated)
 * @param us start vertices (updated)
 * @param vs frontier vertices (updated)
 * @param x original graph
 * @param ft should vertex be visited? (vertex, depth)
 * @param fp action to perform on every visited vertex (vertex, depth)
 */
template <class B=bool, class G, class K, class FT, class FP>
inline void bfsVisitedForEachW(vector<B>& vis, vector<K>& us, vector<K>& vs, const G& x, FT ft, FP fp) {
  for (K u : us) {
    if (vis[u] || !ft(u, K())) continue;
    vis[u] = B(1);
    fp(u, K());
  }
  for (K d=1; !us.empty(); ++d) {
    vs.clear();
    for (K u : us) {
      x.forEachEdgeKey(u, [&](K v) {
        if (vis[v] || !ft(v, d)) return;
        vis[v] = B(1);
        vs.push_back(v);
        fp(v, d);
      });
    }
    swap(us, vs);
  }
}
template <class B=bool, class G, class K, class FT, class FP>
inline void bfsVisitedForEachW(vector<B>& vis, const G& x, K u, FT ft, FP fp) {
  vector<K> us {u}, vs;
  bfsVisitedForEachW(vis, us, vs, x, ft, fp);
}
template <class B=bool, class G, class K, class FT, class FP>
inline vector<B> bfsVisitedForEach(const G& x, K u, FT ft, FP fp) {
  vector<B> vis(x.span());
  bfsVisitedForEachW(vis, x, u, ft, fp);
  return vis;
}

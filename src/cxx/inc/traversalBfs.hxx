#pragma once
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"

using std::vector;
using std::find;




template <class C>
struct TraversalBfsResult {
  C distances;
  C predecessors;
};

template <class C>
auto traversalBfsResult(C dists, C preds) {
  return TraversalBfsResult<C> {dists, preds};
}




template <class G, class K>
auto traversalBfs(float& t, int T, const G& x, K u, size_t alpha, size_t beta) {
  nvgraphHandle_t     h;
  nvgraphGraphDescr_t g;
  struct nvgraphCSRTopology32I_st csr;
  vector<cudaDataType_t> vtype {CUDA_R_32I, CUDA_R_32I};
  vector<int> dists(x.order());
  vector<int> preds(x.order());
  auto ks    = vertexKeys(x);
  auto vfrom = sourceOffsetsAs<int>(x, ks);
  auto efrom = destinationIndicesAs<int>(x, ks);
  int i = find(ks.begin(), ks.end(), u) - ks.begin();

  TRY( nvgraphCreate(&h) );
  TRY( nvgraphCreateGraphDescr(h, &g) );

  csr.nvertices = x.order();
  csr.nedges    = x.size();
  csr.source_offsets      = vfrom.data();
  csr.destination_indices = efrom.data();
  TRY( nvgraphSetGraphStructure(h, g, &csr, NVGRAPH_CSR_32) );
  TRY( nvgraphAllocateVertexData(h, g, vtype.size(), vtype.data()) );

  nvgraphTraversalParameter_t p;
  nvgraphTraversalParameterInit(&p);
  nvgraphTraversalSetDistancesIndex(&p, 0);
  nvgraphTraversalSetPredecessorsIndex(&p, 1);
  nvgraphTraversalSetUndirectedFlag(&p, false);
  if (alpha>0) nvgraphTraversalSetAlpha(&p, alpha);
  if (beta>0)  nvgraphTraversalSetBeta (&p, beta);

  t = measureDuration([&]() { TRY( nvgraphTraversal(h, g, NVGRAPH_TRAVERSAL_BFS, &i, p) ); }, T);
  TRY( nvgraphGetVertexData(h, g, dists.data(), 0) );
  TRY( nvgraphGetVertexData(h, g, preds.data(), 1) );

  TRY( nvgraphDestroyGraphDescr(h, g) );
  TRY( nvgraphDestroy(h) );
  return traversalBfsResult(
    decompressContainer(x, dists, ks),
    decompressContainer(x, preds, ks)
  );
}

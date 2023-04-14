#pragma once
#include <vector>
#include <type_traits>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "transpose.hxx"
#include "pagerank.hxx"

using std::vector;
using std::is_same;




// PAGERANK-CORE
// -------------

template <class G, class T=float>
auto pagerank(float& t, int R, const G& xt, T p=0.85, T E=1e-6, int L=500, const vector<T> *q=nullptr) {
  int N = xt.order();
  nvgraphHandle_t     h;
  nvgraphGraphDescr_t g;
  struct nvgraphCSCTopology32I_st csc;
  cudaDataType_t type = is_same<T, float>::value? CUDA_R_32F : CUDA_R_64F;
  vector<cudaDataType_t> vtype {type, type};
  vector<cudaDataType_t> etype {type};
  vector<T> ranks(N);
  if (N==0) return decompressContainer(xt, ranks);
  auto ks    = vertexKeys(xt);
  auto vfrom = sourceOffsetsAs<int>(xt, ks);
  auto efrom = destinationIndicesAs<int>(xt, ks);
  auto vdata = vertexData(xt, ks, [&](auto v, auto d) { return xt.vertexValue(v)==0? T(1) : T(); });
  auto edata = edgeData(xt, ks, [&](auto v, auto u, auto w) { return T(1)/xt.vertexValue(u); });
  if (q) ranks = compressContainer(xt, *q);

  TRY_NVGRAPH( nvgraphCreate(&h) );
  TRY_NVGRAPH( nvgraphCreateGraphDescr(h, &g) );

  csc.nvertices = xt.order();
  csc.nedges    = xt.size();
  csc.destination_offsets = vfrom.data();
  csc.source_indices      = efrom.data();
  TRY_NVGRAPH( nvgraphSetGraphStructure(h, g, &csc, NVGRAPH_CSC_32) );

  TRY_NVGRAPH( nvgraphAllocateVertexData(h, g, vtype.size(), vtype.data()) );
  TRY_NVGRAPH( nvgraphAllocateEdgeData  (h, g, etype.size(), etype.data()) );
  TRY_NVGRAPH( nvgraphSetVertexData(h, g, vdata.data(), 0) );
  TRY_NVGRAPH( nvgraphSetEdgeData  (h, g, edata.data(), 0) );

  t = measureDurationMarked([&](auto mark) {
    if (q) TRY_NVGRAPH( nvgraphSetVertexData(h, g, ranks.data(), 1) );
    mark([&]() { TRY_NVGRAPH( nvgraphPagerank(h, g, 0, &p, 0, !!q, 1, E, L) ); });
  }, R);
  TRY_NVGRAPH( nvgraphGetVertexData(h, g, ranks.data(), 1) );

  TRY_NVGRAPH( nvgraphDestroyGraphDescr(h, g) );
  TRY_NVGRAPH( nvgraphDestroy(h) );
  return decompressContainer(xt, ranks, ks);
}

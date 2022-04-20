#pragma once
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"

using std::vector;
using std::find;




template <class G, class K>
auto sssp(float& t, int T, const G& x, K u) {
  nvgraphHandle_t     h;
  nvgraphGraphDescr_t g;
  struct nvgraphCSCTopology32I_st csc;
  vector<cudaDataType_t> vtype {CUDA_R_32F};
  vector<cudaDataType_t> etype {CUDA_R_32F};
  vector<float> dists(x.order());
  auto ks    = vertexKeys(x);
  auto vfrom = sourceOffsetsAs(x, int());
  auto efrom = destinationIndicesAs(x, int());
  auto edata = edgeData(x, ks);
  int i = find(ks.begin(), ks.end(), u) - ks.begin();

  TRY( nvgraphCreate(&h) );
  TRY( nvgraphCreateGraphDescr(h, &g) );

  csc.nvertices = x.order();
  csc.nedges    = x.size();
  csc.destination_offsets = vfrom.data();
  csc.source_indices      = efrom.data();
  TRY( nvgraphSetGraphStructure(h, g, &csc, NVGRAPH_CSC_32) );

  TRY( nvgraphAllocateVertexData(h, g, vtype.size(), vtype.data()) );
  TRY( nvgraphAllocateEdgeData  (h, g, etype.size(), etype.data()) );
  TRY( nvgraphSetEdgeData(h, g, edata.data(), 0) );

  t = measureDuration([&]() { TRY( nvgraphSssp(h, g, 0, &i, 0) ); }, T);
  TRY( nvgraphGetVertexData(h, g, dists.data(), 0) );

  TRY( nvgraphDestroyGraphDescr(h, g) );
  TRY( nvgraphDestroy(h) );
  return decompressContainer(x, dists, ks);
}

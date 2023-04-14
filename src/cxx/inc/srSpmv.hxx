#pragma once
#include <vector>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"

using std::vector;




// SR-SPMV
// -------

template <class G, class K>
auto srSpmv(float& t, int T, const G& x, vector<float>& vx, vector<float>& vy, float a, float b) {
  nvgraphHandle_t     h;
  nvgraphGraphDescr_t g;
  struct nvgraphCSRTopology32I_st csr;
  vector<cudaDataType_t> vtype {CUDA_R_32F, CUDA_R_32F};
  vector<cudaDataType_t> etype {CUDA_R_32F};
  vector<float> rslts(x.order());
  auto ks    = vertexKeys(x);
  auto vfrom = sourceOffsetsAs<int>(x, ks);
  auto efrom = destinationIndicesAs<int>(x, ks);
  auto edata = edgeData(x, ks);

  TRY( nvgraphCreate(&h) );
  TRY( nvgraphCreateGraphDescr(h, &g) );

  csr.nvertices = x.order();
  csr.nedges    = x.size();
  csr.source_offsets      = vfrom.data();
  csr.destination_indices = efrom.data();
  TRY( nvgraphSetGraphStructure(h, g, &csr, NVGRAPH_CSR_32) );
  TRY( nvgraphAllocateVertexData(h, g, vtype.size(), vtype.data()) );
  TRY( nvgraphAllocateEdgeData  (h, g, etype.size(), etype.data()) );
  TRY( nvgraphSetVertexData(h, g, vx.data(), 0) );
  TRY( nvgraphSetVertexData(h, g, vy.data(), 0) );
  TRY( nvgraphSetEdgeData  (h, g, edata.data(), 0) );

  t = measureDuration([&]() { TRY( nvgraphSrSpmv(h, g, 0, &a, 0, &b, 1, NVGRAPH_PLUS_TIMES_SR) ); }, T);
  TRY( nvgraphGetVertexData(h, g, rslts.data(), 1) );

  TRY( nvgraphDestroyGraphDescr(h, g) );
  TRY( nvgraphDestroy(h) );
  return decompressContainer(x, rslts, ks);
}

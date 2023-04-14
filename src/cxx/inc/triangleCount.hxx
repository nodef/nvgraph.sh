#pragma once
#include <cstdint>
#include <vector>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"

using std::vector;




template <class G>
auto triangleCount(float& t, int T, const G& x) {
  nvgraphHandle_t     h;
  nvgraphGraphDescr_t g;
  struct nvgraphCSRTopology32I_st csr;
  auto ks    = vertexKeys(x);
  auto vfrom = sourceOffsetsAs<int>(x, ks);
  auto efrom = destinationIndicesAs<int>(x, ks);

  TRY( nvgraphCreate(&h) );
  TRY( nvgraphCreateGraphDescr(h, &g) );

  csr.nvertices = x.order();
  csr.nedges    = x.size();
  csr.source_offsets      = vfrom.data();
  csr.destination_indices = efrom.data();
  TRY( nvgraphSetGraphStructure(h, g, &csr, NVGRAPH_CSR_32) );

  uint64_t count = 0;
  t = measureDuration([&]() { TRY( nvgraphTriangleCount(h, g, &count) ); }, T);

  TRY( nvgraphDestroyGraphDescr(h, g) );
  TRY( nvgraphDestroy(h) );
  return count;
}

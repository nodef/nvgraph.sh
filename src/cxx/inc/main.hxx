#pragma once
#define BUILD   0  // 5 == BUILD_TRACE
#define OPENMP  1
#define NVGRAPH 1

#ifndef TYPE
#define TYPE float
#endif

#include "_main.hxx"
#include "Graph.hxx"
#include "mtx.hxx"
#include "snap.hxx"
#include "output.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"
#include "duplicate.hxx"
#include "transpose.hxx"
#include "dfs.hxx"
#include "deadEnds.hxx"
#include "selfLoop.hxx"
#include "statistics.hxx"
#include "triangular.hxx"
#include "pagerank.hxx"
#include "srSpmv.hxx"
#include "sssp.hxx"
#include "traversalBfs.hxx"
#include "triangleCount.hxx"

#pragma once
#include "_main.hxx"
#include "Graph.hxx"
#include "mtx.hxx"
#include "snap.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"
#include "degree.hxx"
#include "density.hxx"
#include "duplicate.hxx"
#include "transpose.hxx"
#include "dfs.hxx"
#include "depth.hxx"
#include "components.hxx"
#include "sort.hxx"
#include "deadEnds.hxx"
#include "selfLoop.hxx"
#include "identicals.hxx"
#include "chains.hxx"
#include "dynamic.hxx"
#include "pagerank.hxx"
#include "pagerankSeq.hxx"
#include "pagerankMonolithicSeq.hxx"
#include "pagerankCuda.hxx"
#include "pagerankMonolithicCuda.hxx"

#ifndef NVGRAPH_DISABLE
#include "pagerankNvgraph.hxx"
#else
#define pagerankNvgraph pagerankCuda
#endif

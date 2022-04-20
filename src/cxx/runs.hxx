#pragma once
#include <cstdint>
#include <string>
#include <cstring>
#include <ostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include "inc/main.hxx"

using namespace std;




struct RunOptions {
  string error;
  string input;
  string output;
  string format = "json";
  int    repeat = 1;
  bool   full   = false;
};

struct RunSsspOptions : public RunOptions {
  int source = 1;
};

struct RunPagerankOptions : public RunOptions {
  float  alpha     = 0.85;
  float  tolerance = 1e-6;
  int    max_iter  = 500;
};

struct RunTriangleCountOptions : public RunOptions {};

struct RunTraversalBfsOptions : public RunSsspOptions {
  size_t alpha = 0;
  size_t beta  = 0;
};




// RUN-ERROR
// ---------

bool runError(const string& e) {
  if (e.empty()) return false;
  cerr << "error: " << e << '\n';
  return true;
}


// RUN-WRITE
// ---------

void runWrite(stringstream& s, const RunOptions& o) {
  ofstream f(o.output);
  writeBegin(f, o.format);
  f << s.rdbuf();
  writeEnd(s, o.format);
  f.close();
}




// RUN-*-PARSE
// -----------

template <class F>
void runParse(RunOptions& o, int argc, char **argv, F fn) {
  for (int i=2; i<argc; i++) {
    string a = argv[i];
    size_t e = a.find('=');
    string k = a.substr(0, e);
    auto v = [&]() { return e==string::npos? argv[++i] : a.substr(e+1); };
    if (a.find('-')!=0)                o.input  = a;
    else if (k=="-o" || k=="--output") o.output = v();
    else if (k=="-r" || k=="--repeat") o.repeat = stoi(v());
    else if (k=="-f" || k=="--full")   o.full   = true;
    else if (fn(k, v)) continue;
    else { o.error  = "unknown option \"" + k + "\""; break; }
  }
  size_t e = o.output.rfind('.');
  o.format = e==string::npos? o.output.substr(e+1) : "json";
}

void runSsspParse(RunSsspOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if (k=="-s" || k=="--source") o.source = stoi(v());
    else return false;
    return true;
  });
}

void runPagerankParse(RunPagerankOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if      (k=="-a" || k=="--alpha")     o.alpha     = stof(v());
    else if (k=="-t" || k=="--tolerance") o.tolerance = stof(v());
    else if (k=="-i" || k=="--max_iter")  o.max_iter  = stoi(v());
    else return false;
    return true;
  });
}

void runTriangleCountParse(RunTriangleCountOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [](auto k, auto v) { return false; });
}

void runTraversalBfsParse(RunTraversalBfsOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if      (k=="-s" || k=="--source") o.source = stoi(v());
    else if (k=="-a" || k=="--alpha")  o.alpha  = stoi(v());
    else if (k=="-b" || k=="--beta")   o.beta   = stoi(v());
    else return false;
    return true;
  });
}




// RUN-*-VERIFY
// ------------

string runVerify(const RunOptions& o) {
  if (!o.error.empty()) return o.error;
  if (o.input.empty()) return "no input file given";
  if (!existsFile(o.input.c_str())) return string("file \"") + o.input + "\" does not exist";
  if (!(o.format=="json" || o.format=="yaml")) return string("unknown format \"") + o.format + "\"";
  return "";
}

template <class G>
string runSsspVerify(const RunSsspOptions& o, const G& x) {
  string e = runVerify(o);
  if (!e.empty()) return e;
  if (!x.hasVertex(o.source)) return string("source vertex \"") + to_string(o.source) + "\" not in graph";
  return "";
}

string runPagerankVerify(const RunPagerankOptions& o) {
  string e = runVerify(o);
  if (!e.empty()) return e;
  if (o.alpha<0 || o.alpha>1) return "alpha must be between 0 and 1";
  if (o.tolerance<1e-10 || o.tolerance>1) return "tolerance must be between 1e-10 and 1";
  return "";
}

template <class G>
string runTraversalBfsVerify(const RunTraversalBfsOptions& o, const G& x) {
  return runSsspVerify(o, x);
}




// RUN-*-OUTPUT
// ------------

template <class G>
void runOutput(ostream& a, const RunOptions& o, const G& x) {
  writeValue(a, "input", o.input,   o.format);
  writeValue(a, "order", x.order(), o.format);
  writeValue(a, "size",  x.size(),  o.format);
}

template <class G, class C>
void runSsspOutput(ostream& a, const RunSsspOptions& o, const G& x, float t, const C& dists) {
  runOutput(a, o, x);
  writeValue (a, "source",  o.source, o.format);
  writeValue (a, "time_ms", t,        o.format);
  if (o.full) writeValues(a, "distances", dists, o.format);
}

template <class G, class C>
void runPagerankOutput(ostream& a, const RunPagerankOptions& o, const G& x, float t, const C& ranks) {
  runOutput(a, o, x);
  writeValue (a, "alpha",     o.alpha,     o.format);
  writeValue (a, "tolerance", o.tolerance, o.format);
  writeValue (a, "max_iter",  o.max_iter,  o.format);
  writeValue (a, "time_ms",   t,           o.format);
  if (o.full) writeValues(a, "ranks", ranks, o.format);
}

template <class G>
void runTriangleCountOutput(ostream& a, const RunTriangleCountOptions& o, const G& x, float t, uint64_t count) {
  runOutput(a, o, x);
  writeValue(a, "time_ms", t, o.format);
  if (o.full) writeValue(a, "count", count, o.format);
}

template <class G, class C>
void runTraversalBfsOutput(ostream& a, const RunTraversalBfsOptions& o, const G& x, float t, const C& dists, const C& preds) {
  runOutput(a, o, x);
  writeValue (a, "source",  o.source, o.format);
  writeValue (a, "time_ms", t,        o.format);
  if (o.full) writeValues(a, "distances",    dists, o.format);
  if (o.full) writeValues(a, "predecessors", preds, o.format);
}




// RUN-*
// -----

void runSssp(int argc, char **argv) {
  RunSsspOptions o; float t;
  string e, s0; stringstream s(s0);
  runSsspParse(o, argc, argv);
  e = runVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x = readMtxOutDiGraph(o.input.c_str());  println(x);
  e = runSsspVerify(o, x);
  if (runError(e)) return;
  auto dists = sssp(t, o.repeat, x, o.source);
  printf("[%.3f ms] nvgraphSssp\n", t);
  if (o.output.empty()) return;
  runSsspOutput(s, o, x, t, dists);
  runWrite(s, o);
}


void runPagerank(int argc, char **argv) {
  RunPagerankOptions o; float t;
  string e, s0; stringstream s(s0);
  runPagerankParse(o, argc, argv);
  e = runPagerankVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x  = readMtxOutDiGraph(o.input.c_str()); println(x);
  auto xt = transposeWithDegree(x);  print(xt); printf(" (transposeWithDegree)\n");
  auto ranks = pagerank(t, o.repeat, xt, o.alpha, o.tolerance, o.max_iter);
  printf("[%.3f ms] nvgraphPagerank\n", t);
  if (o.output.empty()) return;
  runPagerankOutput(s, o, x, t, ranks);
  runWrite(s, o);
}


void runTriangleCount(int argc, char **argv) {
  RunTriangleCountOptions o; float t;
  string e, s0; stringstream s(s0);
  runTriangleCountParse(o, argc, argv);
  e = runVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x  = readMtxOutDiGraph(o.input.c_str()); println(x);
  auto xl = lowerTriangular(x); print(xl);      printf(" (lowerTriangular)\n");
  uint64_t count = triangleCount(t, o.repeat, xl);
  printf("[%.3f ms] nvgraphTriangleCount\n", t);
  runTriangleCountOutput(s, o, xl, t, count);
  runWrite(s, o);
}


void runTraversalBfs(int argc, char **argv) {
  RunTraversalBfsOptions o; float t;
  string e, s0; stringstream s(s0);
  runTraversalBfsParse(o, argc, argv);
  e = runVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x = readMtxOutDiGraph(o.input.c_str());  println(x);
  e = runTraversalBfsVerify(o, x);
  if (runError(e)) return;
  auto a = traversalBfs(t, o.repeat, x, o.source, o.alpha, o.beta);
  printf("[%.3f ms] nvgraphTraversalBfs\n", t);
  if (o.output.empty()) return;
  runTraversalBfsOutput(s, o, x, t, a.distances, a.predecessors);
  runWrite(s, o);
}

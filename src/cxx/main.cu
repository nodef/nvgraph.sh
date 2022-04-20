#include <string>
#include <stdio.h>
#include "runs.hxx"

using namespace std;




int main(int argc, char **argv) {
  string cmd = argc>1? argv[1] : "help";
  if (cmd=="shortest-path")  cmd = "sssp";
  else if (cmd=="pr")        cmd = "pagerank";
  else if (cmd=="bfs")       cmd = "traversal-bfs";
  else if (cmd=="triangles") cmd = "triangle-count";
  if (cmd=="help") {}
  else if (cmd=="sssp"          ) runSssp(argc, argv);
  else if (cmd=="pagerank"      ) runPagerank(argc, argv);
  else if (cmd=="traversal-bfs" ) runTraversalBfs(argc, argv);
  else if (cmd=="triangle-count") runTriangleCount(argc, argv);
  else printf("error: unknown command \"%s\"\n", cmd.c_str());
  printf("\n");
  return 0;
}

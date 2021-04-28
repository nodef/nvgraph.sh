CLI for nvGraph, which is a GPU-based graph analytics library written by
NVIDIA, using CUDA.<br>
:package: [NPM](https://www.npmjs.com/package/nvgraph.sh),
:smiley_cat: [GitHub](https://github.com/orgs/nodef/packages?repo_name=nvgraph.sh),
:scroll: [Files](https://unpkg.com/nvgraph.sh/).

> Stability: Experimental.

<br>

```bash
## Finds pagerank of vertices
## Returns ranks
$ nvgraph pagerank -a 0.85 -t 1e-6 -o out.json web-Google.mtx

## Traverses breadth-first from source vertex
## Returns distances, predecessors
$ nvgraph traversal-bfs -s 1 -o out.json web-Google.mtx
```

<br>
<br>


## References

- [nvGraph pagerank example, EN605.617, JHU-EP-Intro2GPU](https://github.com/JHU-EP-Intro2GPU/EN605.617/blob/master/module9/nvgraph_examples/nvgraph_Pagerank.cpp)
- [nvGraph pagerank example, CUDA Toolkit Documentation](https://docs.nvidia.com/cuda/archive/10.0/nvgraph/index.html#nvgraph-pagerank-example)
- [nvGraph, CUDA Toolkit Documentation](https://docs.nvidia.com/cuda/archive/10.0/nvgraph/index.html#introduction)
- [RAPIDS nvGraph NVIDIA graph library](https://github.com/rapidsai/nvgraph)
- [pagerank, libcugraph Documentation](https://docs.rapids.ai/api/libcugraph/legacy/namespacecugraph.html#a8e07829e671204ff42aa226f40ff92d5)
- [RAPIDS cuGraph GPU Graph Analytics Library](https://github.com/rapidsai/cugraph)
- [RAPIDS CUDA DataFrame Internals for C++ Developers - S91043](https://developer.download.nvidia.com/video/gputechconf/gtc/2019/presentation/s91043-rapids-cuda-dataframe-internals-for-c++-developers.pdf)
- [Get path of current script when executed through a symlink](https://unix.stackexchange.com/a/17500/166668)

<br>
<br>

![](https://i.imgur.com/nx1C3Uu.jpg)

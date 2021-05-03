CLI for nvGraph, which is a GPU-based graph analytics library written by
NVIDIA, using CUDA.<br>
:package: [NPM](https://www.npmjs.com/package/nvgraph.sh),
:smiley_cat: [GitHub](https://github.com/orgs/nodef/packages?repo_name=nvgraph.sh),
:scroll: [Files](https://unpkg.com/nvgraph.sh/),
:blue_book: [Wiki](https://github.com/nodef/nvgraph.sh/wiki/).

This is for running **nvGraph** functions right from the CLI with graphs in
MatrixMarket format (.mtx) directly. It just needs a x86_64 linux machine
with NVIDIA GPU drivers installed. Execution time, along with the results
can be saved in JSON/YAML file. The [executable code] is written in C++.
You can install this with `npm install -g nvgraph.sh`.

> Stability: Experimental.

<br>

```bash
## Finds single source shortest path from source vertex
## → returns distances
$ nvgraph sssp -o=out.json -f web-Google.mtx -s=1

## Finds pagerank of all vertices
## → returns ranks
$ nvgraph pagerank -o=out.json -f web-Google.mtx -a=0.85 -t=1e-6

## Counts triangles in undirected, lower triangular graph
## → returns count
$ nvgraph triangle-count -o=out.json -f web-Google.mtx

## Traverses breadth-first from source vertex
## → returns distances, predecessors
$ nvgraph traversal-bfs -o=out.json -f web-Google.mtx -s=1
```

<br>
<br>


## Index

| Command         | Action                       |
| --------------- | ---------------------------- |
| [pagerank] | Finds pagerank of all vertices. |
| [sssp] | Finds single source shortest path from source vertex. |
| [traversal-bfs] | Traverses breadth-first from source vertex. |
| [triangle-count] | Counts triangles in undirected, lower triangular graph. |

<br>
<br>


## References

- [nvGraph pagerank example, EN605.617, JHU-EP-Intro2GPU](https://github.com/JHU-EP-Intro2GPU/EN605.617/blob/master/module9/nvgraph_examples/nvgraph_Pagerank.cpp)
- [nvGraph pagerank example, CUDA Toolkit Documentation](https://docs.nvidia.com/cuda/archive/10.0/nvgraph/index.html#nvgraph-pagerank-example)
- [nvGraph, CUDA Toolkit Documentation](https://docs.nvidia.com/cuda/archive/10.0/nvgraph/index.html#introduction)
- [RAPIDS nvGraph NVIDIA graph library](https://github.com/rapidsai/nvgraph)
- [Get path of current script when executed through a symlink](https://unix.stackexchange.com/a/17500/166668)

<br>
<br>

[![](https://img.youtube.com/vi/3s9psf01ldo/maxresdefault.jpg)](https://www.youtube.com/watch?v=3s9psf01ldo)

[executable code]: https://github.com/cppf/nvgraph.sh
[pagerank]: https://github.com/nodef/nvgraph.sh/wiki/pagerank
[sssp]: https://github.com/nodef/nvgraph.sh/wiki/sssp
[traversal-bfs]: https://github.com/nodef/nvgraph.sh/wiki/traversal-bfs
[triangle-count]: https://github.com/nodef/nvgraph.sh/wiki/triangle-count

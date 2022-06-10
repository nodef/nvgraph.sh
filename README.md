CLI for nvGraph, which is a GPU-based graph analytics library written by NVIDIA, using CUDA.<br>
🐚 [Shell](https://www.npmjs.com/package/nvgraph.sh),
📜 [Files](https://unpkg.com/nvgraph.sh/),
📘 [Wiki](https://github.com/nodef/nvgraph.sh/wiki/).

This is for running [nvGraph] functions right from the [CLI] with graphs in
[MatrixMarket] format (`.mtx`) directly. It just needs a [x86_64] linux machine
with NVIDIA GPU drivers installed. Execution time, along with the results can be
saved in [JSON]/[YAML] file. The [executable code] is written in C++. You can
install this with `npm install -g nvgraph.sh`.

**nvGraph**, as mentioned above, is a **GPU-based graph analytics library**
written by *NVIDIA*. It provides four core graph algorithms: Single Source
Shortest Path ([SSSP]), [PageRank], [Triangle count], and Breadth First Search
([BFS]) traversal. Data is loaded into the GPU in Compressed Sparse Row ([CSR])
format, upon which computation is performed. Here, we load the graph stored in
**MatrixMarket format** (a text-based file format for sparse matrices) into a
dynamic graph data structure in the host (CPU) memory, which is then converted
to CSR format and then transferred to the device (GPU) memory. **Computed results**
are then copied back to the host memory, and written to the **output file**
in suitable format (**JSON/YAML**). Note that **measured time** only includes
the time required for **computation** on the GPU.

The **SSSP** algorithm accepts the *source vertex* as an argument, and returns
the *shortest distance* to each vertex form the source vertex. The **PageRank**
algorithm accepts the *damping factor*, *tolerance*, and *max. iterations* as
arguments, and returns the *rank* of each vertex. In addition to ranks of
vertices in case of the **PageRank** algorithm, we like to also obtain additional
analytics of the rank values, i.e., [Lorenz curve], and [Gini coefficient]. The
**Triangle count** algorithm, unsurprisingly, counts the *number of triangles*
in the graph. The **BFS** traversal algorithm accepts the *source vertex* as an
argument, traverses the graph in breadthwise manner, and returns the *distance*
and *predecessor* of each vertex from the source vertex. Note that
**detailed results** are written to the output file only when the **full**
(`-f`) flag is provided.

Alas, **nvGraph** is now no more actively developed. NVIDIA started developing
[cuGraph] a collection of graph analytics that process data found in GPU
Dataframes as part of [RAPIDS].

> Stability: [Experimental](https://www.youtube.com/watch?v=L1j93RnIxEo).

[nvGraph]: https://github.com/rapidsai/nvgraph
[CLI]: https://en.wikipedia.org/wiki/Command-line_interface
[x86_64]: https://en.wikipedia.org/wiki/X86-64
[MatrixMarket]: https://math.nist.gov/MatrixMarket/formats.html
[JSON]: https://developer.mozilla.org/en-US/docs/Learn/JavaScript/Objects/JSON
[YAML]: https://en.wikipedia.org/wiki/YAML
[executable code]: src/cxx
[SSSP]: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
[PageRank]: https://www.geeksforgeeks.org/page-rank-algorithm-implementation/
[Triangle count]: https://www.geeksforgeeks.org/number-of-triangles-in-a-undirected-graph/
[BFS]: https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/
[CSR]: https://www.geeksforgeeks.org/sparse-matrix-representations-set-3-csr/
[Lorenz curve]: https://en.wikipedia.org/wiki/Lorenz_curve
[Gini coefficient]: https://en.wikipedia.org/wiki/Gini_coefficient
[cuGraph]: https://github.com/rapidsai/cugraph
[RAPIDS]: https://rapids.ai/

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
- [nvGraph Library User's Guide](https://docs.nvidia.com/cuda/archive/10.1/pdf/nvGRAPH_Library.pdf)
- [RAPIDS nvGraph NVIDIA graph library](https://github.com/rapidsai/nvgraph)
- [Get path of current script when executed through a symlink](https://unix.stackexchange.com/a/17500/166668)

<br>
<br>

[![](https://img.youtube.com/vi/3s9psf01ldo/maxresdefault.jpg)](https://www.youtube.com/watch?v=3s9psf01ldo)


[pagerank]: https://github.com/nodef/nvgraph.sh/wiki/pagerank
[sssp]: https://github.com/nodef/nvgraph.sh/wiki/sssp
[traversal-bfs]: https://github.com/nodef/nvgraph.sh/wiki/traversal-bfs
[triangle-count]: https://github.com/nodef/nvgraph.sh/wiki/triangle-count

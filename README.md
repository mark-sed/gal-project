# Graph k-coloring with constraints - comparison of greedy algorithm and algorithm using genetic programming
This project was made for Graph algorithms class at Brno University of Technology by Marek Sedláček and Jiří Křištof.

The main purpose of this project is to test out and compare algorithm for k-coloring using genetic programming. Where the focus is on situation similar to register allocation, where some vertices might be constraint to certain color (register class/physical register). A greedy algorithm was chosen as the competing algorithm.

You can run the benchmarks yourself and even adjust them for your specific problem (graph type) and find out how the algorithms compare. If you just want to see some of the results see results section bellow.

## Algorithms used
* __Greedy__ (`-g`) - Uses greedy approach for coloring (deterministic).
* __Genetic programming__ (`-e`) - Uses methods of genetic programming (evolving the coloring) to color the graph (non-deterministic).
* __Genetic programming using heuristics__ (`-h`) - Same as the pure genetic programming approach (`-e`), but the population phenotypes are initialized using linear heuristic algorithm which takes into consideration the constraints and coloring requirements. Also mutation is done only on the incorrect vertices.

## Results
Overall the pure genetic algorithm (`-e`) was very much slower than the greedy algorithm on all test cases.

The heuristic genetic approach (`-h`) on the other hand turned out to be time-wise very much similar to the greedy approach for small and medium size graphs. The additional value with heuristic genetic approach is that the output wasn't alway the same coloring as it was for the greedy (deterministic) approach and thus different coloring can be done which in some cases of register allocation might cause the code to be more suitable. On the other hand the inconsistency of coloring time makes this approach not quite fit for compilation purposes, where deterministic compilation time is often a wanted attribute.

The following graph showcases the computation time based on the graph size:

![graph](https://github.com/mark-sed/gal-project/blob/main/bench_statistics/dense.png?raw=true)

The following graphs shows the way different run times for evolution algorithm compare to the other two:

![graph](https://github.com/mark-sed/gal-project/blob/main/bench_statistics/runs_geh.png?raw=true)

![box graph](https://github.com/mark-sed/gal-project/blob/main/bench_statistics/box_geh.png?raw=true)

Additional graphs can be found in the [`benchmark_statistics`](https://github.com/mark-sed/gal-project/tree/main/bench_statistics) folder.

## How to run this
To compile the program from source use `make` command, which will create this program as `gal.out` binary file.

`gal.out` supports following arguments:
- `--greedy`: flag defines that greedy algorithm should be used,
- `--evolution`: flag defines that evolution algorithm should be used,
- `--heuristic`: flag defines that evolution algorithm with heuristic should be used,
- Note that one of flags described above must be used (or `--benchmark` must be set),
- `--graph <filename>`: mandatory argument, defines name of input file with graph,
- `--constraints <filename>`: optional argument, defines name of input file with constraints,
- `--output <filename>`: mandatory argument, defines name of output file, into which colored graph will be saved,
- `--colors <int>`: defines number of colors to be used in coloring task, mandatory argument,
- `--population`: defines population number, mandatory if flag `evolution` or `heuristic` is used,
- `--benchmark <filename>`: alternative to algorithm flags, filename defines input benchmark description file (described below); with this argument, file defined in `--output` argument will contain results of benchmark in csv format, other arguments are not used,
- `--help`: prints help.
- Note that input graph file and constraints file must follow input graph format (see below)

E.g.: `./gal.out --graph graphs/big.dot --output output/big-colored.dot --colors 6 --evolution --population 20`, which will color graph `graphs/big.dot` using evolution algorithm with 6 colors, population = 20 and save the output to `output/big-colored.dot`.

Output of `gal.out` (dot file) can be converted into an image using `make dot DOT=path_to_dot_file`.

## Input graph and constraint format

The input graph format is a subset of dot language, where vertices can be only numbers and all graphs are undirected.
Anything after second vertex is ignored, so only 1 edge can be on one line.

The constraint file format is very similar to the input graph format, where the first number is a vertex, but this is
then followed by `:` symbol (not `--` like in graph format) and after this is one color from the set of colors
this vertex can be colored in. Each line can contain only one constraint (see section bellow for example file). 

_Note_ that the file parsing ends after finding `}` on its own line (line with just this one symbol).

### Graph generator
Graph generator can be used for easy generating of graph in the input format. Python 3 interpreter must be installed to run the script. You can describe properties of the desired graph using these arguments:
- `-nn/--nodes-number <int>`: specifies desired number of nodes of graph, this argument is compulsory,
- `-en/--edges-number <int>`: specifies desired number of edges in graph, note that if number is less than minimum/maximum number of edges in unoriented connected graph, this value is changed; it could also be changed if any of flags described below is used,
- `-sf/--sparse-flag`: specifies, that desired graph should be "sparse",
- `-df/--dense-flag`: specifies, that desired graph should be "dense",
- `-cn/--constraints-num <int>`: defines number of constraints to be ,generated, defaultly 0,
- `-ccn/--constraints-colors-num <int>`: number of colors to be used in constraints,
- `-scf/--strict-constraint-fulfill`: if flag is set, then constraints are generated so that there is a solution of graph coloring,
- `-gf/--graph-filename <str>`: defines name of file into which graph dot representation will be saved, defaultly `graph.dot` ,
- `-cf/--constraints-filename <str>`: defines name of file into which constraints dot representation will be saved, defaultly `constraints.dot`,
- `-p/--print`: if flag is set, then graph and constraints are both saved into specified files and printed to stdout,
- `-con/--collision-num <int>`: defines minimum number of collisions to be generated in graph, defaultly 0 (which does not mean there will be no collision),
- `-s/--stats`: if flag is set, then statistics about generated graph will be printed,
- `-h/--help`: prints help for the graph generator script. 

Note that output graph is always connected, meaning there is always n-1 edges (where n is number of nodes). If number of edges is not specified and no flag is set, then output graph is degraded to linked list. Maximum constraints that can be generated is defined as `nodes_number * constraints_colors_num`, e.g. for 5 nodes and 4 colors, 20 is maximum number of constraints that can be generated. 

### Example input graph
```
graph g2 {
	1 -- 2;
	2 -- 3;
	3 -- 1;
	2 -- 4;
}
```

### Example constraint file
```
constraints g2 {
	0: 1
	0: 2
	2: 2
	3: 0
	3: 1
	3: 2
	4: 
}
```
This file sets the following constraints: 
* Color of vertex 0 can be one of the following: 1, 2.
* Vertex 1 can have any color.
* Color of vertex 2 can be one of the following: 2.
* Color of vertex 3 can be one of the following: 0, 1, 2.
* Vertex 4 can have any color.

## Benchmark
Benchmark is special mode of the app, in which algorithms can be evaluated. For statistics visualisation, use script `evaluate.py`. This script is described below. Creation of input csv for benchmarking was automated by script `csv_creator.ipynb`. You can find test files with graphs and constraints at URL http://www.stud.fit.vutbr.cz/~xkrist22/GAL/test_data/. These can be used with benchmark test input file.

### Input file format
Benchmark input file uses extended csv format, where lines are options for one benchmarking. Each file must define these values in given order:
1. `id`: unique integer identificator of given benchmark,
2. `algorithm`: one character specifing algorithm to be used, should be one of the value `g`, `e` or `h`,
3. `graph_file`: string defining name of file in which graph for benchmarking is used,
4. `constraints_file`: string defining name of file in which constraints for benchmarking is used, if you do not want to use constraint file, then fill value `X` for this column,
5. `colors`: integer number of colors to be used in graph coloring algorithm,
6. `population`: integer number defining population size, if `g` algorithm is set, then number will not be used (but still there must be some number),
7. `repetition`: integer number defines how many times each benchmark should be realised.

Lines which does not meet input format are skipped. You can use line comments, which will be printed to stdout during processing input csv file. These comments starts with `#` symbol. Example of input file is below:
```
# dependency between duration of each algorithm and number of nodes in sparse graph
0,g,bench_graphs/sparse_20.dot,x,6,0,5
1,e,bench_graphs/sparse_20.dot,x,6,20,5
2,h,bench_graphs/sparse_20.dot,x,6,20,5
```
With this input file, greedy algorithm and evolution with heuristic will be run `5` times (evolution is run only once). As input graph is used graph stored in file `bench_graphs/sparse_20.dot`. No constraint file is set. For coloring will be used `6` colors, population is `20`.

### Output file format
Output file is in csv format and contains results of benchmark. Each line has these columns:
1. `id`: identificator of given benchmark settings (used as foreign key to match exactly one input benchmark settings), note that in output file, `id` is not unique,
2. `time`: column contains information about algorithm duration
3. `success`: bool value defining if coloring was sucessful or not
4. `node_num`: number of nodes of graph used in benchmark
5. `edge_num`: number of edges of graph used in benchmark
6. `constaints_num`: number of constraints generated for graph

### Script `evaluate.py`
This script can be used to visualise results of benchmarking. Script will plot line chart according to given arguments:
- `-bi/--benchmark-input`: defines file used as input to gal.out in mode benchmark, by default `in.csv`
- `-bo/--benchmark-output`: defines file used as output from gal.out in mode benchmark, by default `out.csv`
- `-si/--start-index`: first index of relevant data, mandatory argument
- `-ei/--ending-index`: last index of relevant data, mandatory argument
- - `-x/--x-axis-var`: defines variable which will be plotted to x axe, mandatory argument
- - `-y/--y-axis-var`: defines variable which will be plotted to y axe, mandatory argument
- `-of/--output-file`: defines output file into which line chart will be plotted, by default `out.png`

# GAL Project: Graph k-coloring 

# How to run this
To compile the program from source use `make` command, which will create this program as `gal.out` binary file.

`gal.out` takes following arguments:
1. path to the input graph (which follows input graph format - see bellow),
2. path to the output dot graph file containing colored graph,
3. number of available colors,
4. algorithm to use for coloring, which can be one of the following:
    * `-g` - greedy algorithm,
    * `-e` - evolution algorithm using genetic programming,
5. optionally constraints file can be passes in as the last argument to constrain certain vertices to set of colors.

E.g.: `./gal graphs/big.dot output/big-colored.dot 6 -e`, which will color graph `graphs/big.dot` using evolution 
algorithm with 6 colors and save the output to `output/big-colored.dot`.

Output of `gal.out` (dot file) can be converted into an image using `make dot DOT=path_to_dot_file`.

# Input graph and constraint format

The input graph format is a subset of dot language, where vertices can be only numbers and all graphs are undirected.
Anything after second vertex is ignored, so only 1 edge can be on one line.

The constraint file format is very similar to the input graph format, where the first number is a vertex, but this is
then followed by `:` symbol (not `--` like in graph format) and after this is one color from the set of colors
this vertex can be colored in. Each line can contain only one constraint (see section bellow for example file). 

_Note_ that the file parsing ends after finding `}` on its own line (line with just this one symbol).

## Graph generator
Graph generator can be used for easy generating of graph in the input format. Python3 interpreter must be installed to run the script. You can describe properties of the desired graph using these arguments:
- `-nn/--nodes-number <int>` specifies desired number of nodes of graph, this argument is compulsory,
- `-en/--edges-number <int>` specifies desired number of edges in graph, note that if number is less than minimum/maximum number of edges in unoriented connected graph, this value is changed; it could also be changed if any of flags described below is used,
- `-sf/--sparse-flag` specifies, that desired graph should be "sparse",
- `-df/--dense-flag` specifies, that desired graph should be "dense",
- `-cn/--constraints-num <int>` defines number of constraints to be ,generated, defaultly 0,
- `-cc/--constraints-colors <list>` list of colors to be used in constraints, there must be at least 4 different colors,
- `-scf/--strict-constraint-fulfill` if flag is set, then constraints are generated so that there is a solution of graph coloring,
- `-gf/--graph-filename <str>` defines name of file into which graph dot representation will be saved, defaultly `graph.dot` ,
- `-cf/--constraints-filename <str>` defines name of file into which constraints dot representation will be saved, defaultly `constraints.dot`,
- `-p/--print` if flag is set, then graph and constraints are both saved into specified files and printed to stdout. 

Note that output graph is always connected, meaning there is always n-1 edges (where n is number of nodes). If number of edges is not specified and no flag is set, then output graph is degraded to linked list. Maximum constraints that can be generated is defined as `nodes_number * len(constraints_colors)`, e.g. for 5 nodes and 4 colors, 20 is maximum number of constraints that can be generated. 

## Example input graph
```
graph g2 {
	1 -- 2;
	2 -- 3;
	3 -- 1;
	2 -- 4;
}
```

## Example constraint file
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

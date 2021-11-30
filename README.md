# GAL Project: Graph k-coloring 

# How to run this
To compile the program from source use `make` command, which will create this program as `gal.out` binary file.

`gal.out` takes following arguments:
1. path to the input graph (which follows input graph format - see bellow),
2. path to the output dot graph file containing colored graph,
3. number of available colors,
4. algorithm to use for coloring, which can be one of the following:
    * `-g` - greedy algorithm,
    * `-e` - evolution algorithm using genetic programming.

E.g.: `./gal graphs/big.dot output/big-colored.dot 6 -e`, which will color graph `graphs/big.dot` using evolution 
algorithm with 6 colors and save the output to `output/big-colored.dot`.

Output of `gal.out` (dot file) can be converted into an image using `make dot DOT=path_to_dot_file`.

# Input graph format

The input graph format is a subset of dot language, where vertices can be only numbers and all graphs are undirected.
Anything after second vertex is ignored, so only 1 edge can be on one line.

_Note_ that the file parsing ends after finding `}` on its own line (line with just this one symbol).

## Example input graph
```
graph g2 {
	1 -- 2;
	2 -- 3;
	3 -- 1;
	2 -- 4;
}
```


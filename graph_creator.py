"""
This script implements graph generator. Graph is generated according to data given
as arguments. Script supports these arguments:
    -nn/--nodes-number <int> define number of nodes graph will have, compulsory
    -en/--edges-number <int> define number of edges, defaultly is set to minimum
        number of edges possible for connected graph; can be overwritten 
        by flags
    -sf/--sparse-flag: if flag is set, then graph will be "sparse", number of edges
        is ignored if this flag is set
    -df/--dense-flag: if flag is set, then graph will be "dense", number of edges
        is ignored if this flag is set
    -cn/--constraints-num <int> define maximum number of constraints to be defined, defaultly
        0, maximum nodes-number * len(constraints-colors)
    -cc/--constraints-colors <LIST> list of colors to be used in constraint generator, 
        note that minimum is 4 colors, defaultly 0 1 2 3
    -scf/--strict-constraint-fulfill if flag is set, then constraints are generated so that
        there is a solution of graph coloring
    -gf/--graph-filename <STR> defines name of file into which graph will be stored,
        defaultly "graph.dot"
    -cf/--constraints-filename <STR> defines name of file into which constraints will
        be saved. defaultly "constraints.dot" 
    -p/--print
Script generates graph and constraints and saved them in subset of dot format (more
described in documentation) into specified file. See more details on README.md
"""

__author__ = "Jiří Křištof"
__contact__ = "xkrist22@stud.fit.vutbr.cz"
__date__ = "29-11-2021"


import random
from typing import *
import argparse
from os import makedirs
from os.path import dirname
from time import sleep


class graph_creator:
    @classmethod
    def validate_edge_num(cls, node_num: int, edge_num: int, sparse: bool = None) -> int:
        """
        Method check, if nuber of edges is bigger than smallest possible value
        and smaller than biggest possible value. If it is smaller/bigger, then
        method returns min/max number of edges, else returns specified number.

        Args:
            node_num (int): number of nodes in graph
            edge_num (int): number of edges in graph
            sparse (bool): if is set to None, graph will be generated according
                to given values, if is True, then graph will be sparse, else
                graph will be dense
        
        Returns:
            int: validated number in range <min;max>, if sparse is set to True,
                then edge_num is in <min;q1>, if is set to True, edge_num is in
                <q3;max> interval

        """

        max_edges = (node_num * (node_num - 1)) // 2
        min_edges = node_num - 1

        # check if number of edges is in interval <min;max>, eventually 
        #   replace value for one of boundary
        if edge_num < min_edges:
            edge_num = min_edges
        elif edge_num > max_edges:
            edge_num = max_edges

        # change number of edges according to --sparse or --dense flag
        if sparse is not None:
            mean = (max_edges + min_edges) // 2
            q1 = int(0.6 * ((min_edges + mean) // 2)) + 1
            q3 = (max_edges + mean) // 2

            if sparse and edge_num < q1 and edge_num > min_edges:
                # graph is sparse and number of edges is in interval <min+1;q1>
                return edge_num
            elif sparse:
                # graph should be sparse, but given number of edges cannot be used
                return q1
            elif not sparse and edge_num > q3:
                # graph should be dense and edge num is in interval <q3;max>
                return edge_num
            else:
                # graph should be dense, but given number of edges cannot be used
                return q3

        return edge_num


    @classmethod
    def encode_graph(cls, graph: List[Tuple[int, int]]) -> str:
        """
        Method encodes graph in internal list into subset of dot
        format

        Args:
            graph (List[Tuple[int, int]]): list of pairs of nodes creating
                an edge
        
        Returns:
            str: dot subset representation of graph (see doc for more describtion)
        """
        
        # create header of graph representation
        str_graph = "graph g {\n"
        for edge in graph:
            # for each pair of nodes, create edge representation
            str_graph += "\t{} -- {}\n".format(edge[0], edge[1])
        # create footer
        str_graph += "}\n"
        return str_graph


    def generate_graph(self, node_num: int, edge_num: int, sparse: bool = None) -> str:
        """
        Method generates graph defined by number of nodes and edges

        Args:
            node_num (int): number of nodes
            edge_num (int): number of edges
            sparse (bool): if is set to None, graph will be generated according
                to given values, if is True, then graph will be sparse, else
                graph will be dense
        
        Returns:
            str: graph encoded in subset of dot format (explained more in doc)
        """
        # there must be at least one node, otherwise return empty graph
        if node_num <= 0:
            return graph_creator.encode_graph([])

        graph = []
        edges_completed = 0
        nodes = list(range(node_num))

        # check if constraints are satisfied
        edge_num = graph_creator.validate_edge_num(node_num, edge_num, sparse)
        
        # iterate until edge_num edges was not generated
        while (edges_completed < edge_num):
            # each iteration shuffle list of nodes       
            random.shuffle(nodes)

            # create edge between nth and nth+1 node
            for i in range(node_num - 1):
                # check if edge is not already in graph - multigraphs are not allowed
                if (nodes[i], nodes[i+1]) not in graph and (nodes[i+1], nodes[i]) not in graph:
                    graph.append((nodes[i], nodes[i+1]))
                    edges_completed += 1
                if (edges_completed >= edge_num):
                    break
        
        # save graph for constraint generator
        self.__graph = graph
        self.__graph_str = graph_creator.encode_graph(graph)
        return self.__graph_str


    @classmethod
    def encode_constraints(cls, constraints: List[Tuple[int, int]]) -> str:
        """
        Method encodes internal list of constraints into subset of dot
        format

        Args:
            constraints (List[Tule[int, int]]): list of pairs containing node and color constraint;
                nodes can repeat in more pairs
        
        Returns:
            str: encoded version of constraints
        """

        # create header of graph representation
        str_constraints = "constraints g {\n"
        for constraint in constraints:
            # for each pair of nodes, create edge representation
            str_constraints += "\t{}: {}\n".format(constraint[0], constraint[1])
        # create footer
        str_constraints += "}\n"
        return str_constraints

    
    def generate_constraints(self, node_num: int, constraints_num: int, constraints_colors: List[int], strict: bool = False) -> str:
        """
        Method generates constraints according to given arguments. 

        Args:
            node_num (int): number of nodes in graph
            constraints_num (int): number of contraints to be generated
            constraints_colors (List[int]): list of supported colors
            strict (bool): if flag is set, then color constraints is generated so all these can be fulfill

        Returns:
            str: dot representation of constraints
        """

        constraints = []
        completed_num = 0

        # remove duplicate colors
        constraints_colors = list(dict.fromkeys(constraints_colors))

        if len(constraints_colors) < 4:
            raise ValueError("You must define at least 4 different colors")

        if len(constraints_colors) * node_num < constraints_num:
            raise ValueError("Cannot create {} constraints, maximum for this graph is {}".format(constraints_num, len(constraints_colors) * node_num))
        
        if len(constraints_colors) * node_num == constraints_num:
            # there is all constraints (every node is constrained to every color), aka no constraints
            self.__constraints_str = graph_creator.encode_constraints([])
            return self.__constraints_str

        while(completed_num < constraints_num):
            color = random.sample(constraints_colors, 1)[0]
            node = random.randint(0,node_num-1)
            if not strict and (node, color) not in constraints:
                # non-strict 
                constraints.append((node, color))
                completed_num += 1
    
            elif strict and (node, color) not in constraints:
                # strict - check existing nodes and colors
                constraint_ok = True

                # for edges starting from given node
                for neighbor in [x for x in self.__graph if x[0] == node]:
                    if (neighbor[1], color) in constraints and len([x for x in constraints if x[0] == neighbor[1]]) == 1:
                        # if neighbor has only one constraint with same color, then skip this combination
                        constraint_ok = False
                        break

                # for edges ending in given node
                for neighbor in [x for x in self.__graph if x[1] == node]:
                    if (neighbor[0], color) in constraints and len([x for x in constraints if x[0] == neighbor[0]]) == 1:
                        # if neighbor has only one constraint with same color, then skip this combination
                        constraint_ok = False
                        break

                if constraint_ok:
                    # no neighbors has this color or no collision
                    constraints.append((node, color))
                    completed_num += 1

        self.__constraints_str = graph_creator.encode_constraints(constraints)
        return self.__constraints_str


    @classmethod
    def write_data(csl, data: str, filename: str) -> None:
        """
        Method saves graph into specified file

        Args:
            graph (str): dot representation of graph
            filename (str): name of file into which graph will be saved
        """

        with open(filename, "w+") as f:
            f.write(data)


    def __str__(self):
        return self.__graph_str + self.__constraints_str


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Script for generating graphs")
    parser.add_argument(
        "--nodes-number", "-nn",
        action="store",
        dest="nodes",
        required=True,
        help="Define number of nodes"
    )
    parser.add_argument(
        "--edges-number", "-en",
        action="store",
        dest="edges",
        default=-1,
        help="Define number of edged"
    )
    parser.add_argument(
        "--sparse-flag", "-sf",
        action="store_true",
        dest="sparse",
        default=None,
        help="if is set, graph will be sparse"
    )
    parser.add_argument(
        "--dense-flag", "-df",
        action="store_false",
        dest="sparse",
        default=None,
        help="if is set, graph will be dense"
    )
    parser.add_argument(
        "--constraints-num", "-cn",
        action="store",
        dest="constraints_num",
        default=0,
        help="number of constraints to be generated, defaultly 0"
    )
    parser.add_argument(
        "--constraints-colors", "-cc",
        action="store",
        dest="constraints_colors",
        default=[0, 1, 2, 3],
        nargs="+",
        help="list of colors that can be used in constraints, defaultly 0 1 2 3"
    )
    parser.add_argument(
        "--strict-constraint-fulfill", "-scf",
        action="store_true",
        dest="strict_constraint_fulfill",
        default=False,
        help="if flag is set, then colors constraints are generated in way all these can be fulfilled"
    )
    parser.add_argument(
        "--graph-filename", "-gf",
        action="store",
        dest="graph_filename",
        default="graph.dot",
        help="file into which graph will be saved, defaultly \"graph.dot\""
    )
    parser.add_argument(
        "--constraints-filename", "-cf",
        action="store",
        dest="constraints_filename",
        default="constraints.dot",
        help="file into which constraints will be saved, defaultly \"constraints.dot\""
    )
    parser.add_argument(
        "--print", "-p",
        action="store_true",
        dest="print",
        default=False,
        help="if flag is set, then results will be both saved into files and written to stdout"
    )


    arguments = parser.parse_args()

    gc = graph_creator()

    graph_creator.write_data(gc.generate_graph(int(arguments.nodes), int(arguments.edges), arguments.sparse), arguments.graph_filename)
    graph_creator.write_data(gc.generate_constraints(int(arguments.nodes), int(arguments.constraints_num), arguments.constraints_colors, arguments.strict_constraint_fulfill), arguments.constraints_filename)
    
    if arguments.print:
        print(gc)

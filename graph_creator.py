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
    __CONSTRAINTS_PER_COLLISION = 3

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
        
        self.__node_num = node_num
        self.__edge_num = edge_num

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

    
    def generate_constraints(self, node_num: int, constraints_num: int, constraints_colors_num: int, strict: bool = False, collision_num: int = 0) -> str:
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

        if strict:
            # check maximum colors needed for constraints -- maximum edges from node + 1
            for node in range(node_num):
                neighbors = []
                for neighbor in [x for x in self.__graph if x[0] == node]:
                    neighbors.append(neighbor[1])
                for neighbor in [x for x in self.__graph if x[1] == node]:
                    neighbors.append(neighbor[0])
                if len(neighbors) > constraints_colors_num:
                    constraints_colors_num = len(neighbors)
        
        # save number of colors to stats
        self.__color_num = constraints_colors_num

        if collision_num != 0 and constraints_num - collision_num < node_num * constraints_colors_num:
            constraints_num = (node_num * constraints_colors_num) - collision_num

        if constraints_num + collision_num > constraints_colors_num * node:
            constraints_num = (constraints_colors_num * node) - collision_num

        # save number of constraints to stats
        self.__constraints_num = constraints_num

        while completed_num < constraints_num - collision_num:
            color = random.randint(0,constraints_colors_num-1)
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

        completed_num = 0
        while completed_num < collision_num:
            # take random node
            node = random.randint(0,node_num-1)
            
            # get colors constraints of nodes
            node_colors = []
            for constraint in [c for c in constraints if c[0] == node]:
                node_colors.append(constraint[1])

            # if there is no constraint, then do not risk and select another node :-)
            if len(node_colors) == 0:
                continue

            # get all colors that can be used in collision
            collision_colors = []
            for neighbor in [x for x in self.__graph if x[0] == node]:
                for color in [c for c in constraints if c[0] == neighbor[1]]:
                    collision_colors.append(color[1])
            for neighbor in [x for x in self.__graph if x[1] == node]:
                for color in [c for c in constraints if c[0] == neighbor[0]]:
                    collision_colors.append(color[1])

            # select specific collision number (if there is any new)
            collision_colors = [c for c in collision_colors if c not in node_colors]
            if collision_colors != []:
                color = random.sample(collision_colors, 1)[0]
                constraints.insert(0, (node, color))
                completed_num += 1

        # count collision stat
        collisions = 0
        for edge in self.__graph:
            starting_constraint_colors = []
            for color in [c for c in constraints if c[0] == edge[0]]:
                starting_constraint_colors.append(color[1])
            ending_constraint_colors = []
            for color in [c for c in constraints if c[0] == edge[1]]:
                ending_constraint_colors.append(color[1])
            
            collisions += len([c for c in starting_constraint_colors if c in ending_constraint_colors])

        self.__collisions = collisions
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


    def print_stats(self):
        print("================ STATS ================")
        print("Number of nodes: {}".format(self.__node_num))
        print("Number of edges: {}".format(self.__edge_num))
        print("Number of constraints: {}".format(self.__constraints_num))
        print("Number of colors: {}".format(self.__color_num))
        print("Number of collisions: {}".format(self.__collisions))
        print("Density of graph: {:.3f} %".format((200 * self.__edge_num)/(self.__node_num * (self.__node_num -1))))


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
        "--constraints-colors-num", "-ccn",
        action="store",
        dest="constraints_colors_num",
        default=1,
        help="number of colors used in constraints, defaultly 4"
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
    parser.add_argument(
        "--collision-num", "-con",
        action="store",
        dest="collision_num",
        default=0,
        help="Minimum number of collisions to be generated, defaultly 0 (note that this is minimum, not maximum)"
    )
    parser.add_argument(
        "--stats", "-s",
        action="store_true",
        dest="print_stats",
        default=False,
        help="If flag is set, then statistics about generated graph will be printed"
    )

    arguments = parser.parse_args()

    gc = graph_creator()

    graph_creator.write_data(gc.generate_graph(int(arguments.nodes), int(arguments.edges), arguments.sparse), arguments.graph_filename)
    graph_creator.write_data(gc.generate_constraints(int(arguments.nodes), int(arguments.constraints_num), int(arguments.constraints_colors_num), arguments.strict_constraint_fulfill, int(arguments.collision_num)), arguments.constraints_filename)
    
    if arguments.print:
        print(gc)

    if arguments.print_stats:
        gc.print_stats()
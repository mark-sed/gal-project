"""
This script implements graph generator. Graph is generated according to data given
as arguments. Script supports these arguments:
    -n/--nodes <int> define number of nodes graph will have, compulsory
    -e/--edges <int> define number of edges, defaultly is set to minimum
        number of edges possible for connected graph; can be overwritten 
        by flags
    -s/--sparse: if flag is set, then graph will be "sparse", number of edges
        is ignored if this flag is set
    -d/--dense: if flag is set, then graph will be "dense", number of edges
        is ignored if this flag is set
Script generates graph and prints it to stdout in subset of dot format (more
described in documentation)
"""

__author__ = "Jiří Křištof"
__contact__ = "xkrist22@stud.fit.vutbr.cz"
__date__ = "29-11-2021"


import random
from typing import *
import argparse


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
            str: dot subset representation of graph
        """
        
        # create header of graph representation
        str_graph = "graph g {\n"
        for edge in graph:
            # for each pair of nodes, create edge representation
            str_graph += "\t{} -- {}\n".format(edge[0], edge[1])
        # create footer
        str_graph += "}\n"
        return str_graph


    @classmethod
    def generate_graph(cls, node_num: int, edge_num: int, sparse: bool = None) -> str:
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
            return cls.encode_graph([])

        graph = []
        edges_completed = 0
        nodes = list(range(node_num))

        # check if constraints are satisfied
        edge_num = cls.validate_edge_num(node_num, edge_num, sparse)
        
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
        
        return cls.encode_graph(graph)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Script for generating graphs")
    parser.add_argument("--nodes", "-n", action="store", dest="nodes", required=True, help="Define number of nodes")
    parser.add_argument("--edges", "-e", action="store", dest="edges", default=-1, help="Define number of edged")
    parser.add_argument("--sparse", "-s", action="store_true", dest="sparse", default=None, help="if is set, graph will be sparse")
    parser.add_argument("--dense", "-d", action="store_false", dest="sparse", default=None, help="if is set, graph will be dense")
    arguments = parser.parse_args()

    print(graph_creator.generate_graph(int(arguments.nodes), int(arguments.edges), arguments.sparse))

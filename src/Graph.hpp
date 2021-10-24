#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <random>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define MIN_VERTICES 1
#define MAX_ATTEMPS_TO_CREATE_EDGE 15

typedef boost::property<boost::vertex_color_t, float> vertex_property;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
    vertex_property, boost::no_property> graph_t;
typedef boost::graph_traits<graph_t>::edge_iterator edge_iterator_t;

class Graph{
    public:
        /**
         * Method generates random undirected graph
         */
        Graph(int num_of_vertices, int num_of_edges);

        /**
         * Method create representation of graph described
         * by in the given file
         */
        Graph(std::string filename);

        /**
         * Method returns boost representation of graph
         */
        graph_t get_graph_representation();

        /**
         * Method prints graph as edge list
         */
        void print_graph();

        /**
         * Method prints graph to stdout in graphviz format
         */
        void save_graph();

    private:
        // boost representation of graph
        graph_t graph;
};
#include <iostream>
#include "graph.hpp"

int main(int argc, char *argv[]) {
    if(argc == 1) {
        auto g = new Graph(5);
        g->add_edge(0, 1);
        g->add_edge(0, 2);
        g->add_edge(0, 3);
        g->add_edge(1, 2);
        g->add_edge(3, 2);

        g->kcolor_greedy(3);
        g->create_dot("g1", "g.dot");
    }
    else {
        auto g = new Graph(argv[1]);
        g->kcolor_greedy(5);
        g->create_dot("loaded", "loaded.dot");
    }

    return 0;
}
#include <iostream>
#include "graph.hpp"

int main(int argc, char *argv[]) {
    auto g = new Graph(5);
    g->add_edge(0, 1);
    g->add_edge(0, 2);
    g->add_edge(0, 3);
    g->add_edge(1, 2);
    g->add_edge(3, 2);

    g->kcolor(100);
    g->create_dot("g1", "g.dot");

    return 0;
}
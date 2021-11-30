#include <iostream>
#include "graph.hpp"

int main(int argc, char *argv[]) {
    /*if(argc == 1) {
        std::cout << "Creating custom small graph, colored output is in g.dot file." << std::endl
                  << "If you want to supply your own graph pass it as the 1st argument and desired output "
                  << "path as the second argument" << std::endl;
        auto g = new Graph(5);
        g->add_edge(0, 1);
        g->add_edge(0, 2);
        g->add_edge(0, 3);
        g->add_edge(1, 2);
        g->add_edge(3, 2);

        g->kcolor_greedy(3);
        g->create_dot("g1", "g.dot");
    }
    else */
    if (argc == 5) {
        auto g = new Graph(argv[1]);
        // TODO: Check conversion
        int k = atoi(argv[3]);
        if(argv[4] == std::string("-g")) {
            g->kcolor_greedy(k);
        }
        else if(argv[4] == std::string("-e")) {

        }
        else {
            std::cerr << "ERROR: Unknwon algorithm '" << argv[3] << "'" << std::endl;
            return 1;
        }
        g->create_dot("colored_graph", argv[2]);
    }
    else {
        std::cout << "Usage: " << argv[0] << " input_graph.dot output_colored_graph.dot <k> -<algorithm>" << std::endl
                  << "where <k> is the number of colors and <algorithm> can be one of the following values:" << std::endl
                  << "\t-g\tGreedy algorithm" << std::endl
                  << "\t-e\tEvolution algorithm using genetic programming" << std::endl
                  ;
    }

    return 0;
}
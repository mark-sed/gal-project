#include <iostream>
#include "graph.hpp"

int main(int argc, char *argv[]) {
    if (argc == 5 || argc == 6) {
        const char *con_file = (argc == 6 ? argv[5] : nullptr);
        auto g = new Graph(argv[1], con_file);
        // TODO: Check conversion
        int k = atoi(argv[3]);
        if(argv[4] == std::string("-g")) {
            g->kcolor_greedy(k);
            std::cout << "Graph correctly colored: " << std::boolalpha << g->is_correctly_colored() << std::endl;
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
        std::cout << "Usage: " << argv[0] << " input_graph.dot output_colored_graph.dot <k> -<algorithm> [constraint_file]" << std::endl
                  << "where <k> is the number of colors and <algorithm> can be one of the following values:" << std::endl
                  << "\t-g\tGreedy algorithm" << std::endl
                  << "\t-e\tEvolution algorithm using genetic programming" << std::endl
                  << "The constraint_file is optional file containing constraints" << std::endl;
                  ;
    }

    return 0;
}
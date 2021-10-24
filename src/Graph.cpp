#include "Graph.hpp"

Graph::Graph(int num_of_vertices, int num_of_edges) {
    // check if there will be at least one vertex
    if (num_of_vertices < MIN_VERTICES) {
        throw std::invalid_argument("Numbers od vertices must be positive number");
    }

    // check if desired number of edges meets limitations
    if ((num_of_edges > num_of_vertices*(num_of_vertices-1)) / 2) {
        throw std::invalid_argument("Required number of edges cannot be created");
    }

    // generate given number of vertices
    for (int i = 0; i < num_of_vertices; i++) {
        boost::add_vertex(this->graph);
    }

    // reset prng seed
    srand(time(0));

    // generate randomly edges
    for (int i = 0; i < num_of_edges; i++) {
        // create counter of attemps to create edge
        int num_of_tries = 0;

        while (true) {
            // generate randomly start and end vertex for new edge
            int start_vertex = rand() % (num_of_vertices + 1);
            int end_vertex = rand() % (num_of_vertices + 1);
            bool is_addded = boost::add_edge(start_vertex, end_vertex, this->graph).second;
            num_of_tries++;

            // if edge is add, or maximum attemps to create edge passed, 
            // start generating new edge
            if (is_addded || num_of_tries > MAX_ATTEMPS_TO_CREATE_EDGE) {
                break;
            }
        }
    }
}

Graph::Graph(std::string filename) {
    //TODO
}

graph_t Graph::get_graph_representation() {
    return this->graph;
}

void Graph::print_graph() {
    std::pair<edge_iterator_t, edge_iterator_t> ei = edges(this->graph);
 
    std::cout << "* Number of edges = " << num_edges(this->graph) << "\n";
    std::cout << "* Edge list:\n";
 
    for (edge_iterator_t it = ei.first; it != ei.second; ++it ) {
        std::cout << *it << std::endl;
    }

    std::cout << std::endl;
}

void Graph::save_graph() {
    boost::write_graphviz(std::cout, this->graph);
}
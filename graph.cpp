#include <iostream>
#include <sstream>
#include <fstream>
#include "graph.hpp"

Graph::Graph(int size) : colors_used{size}, size{size} {
    this->adj = new std::vector<int>[size];
    this->colors = new Color[size]();
}

void Graph::add_edge(int src, int dst) {
    adj[src].push_back(dst);
    adj[dst].push_back(src);
}

void Graph::print() {
    for (int i = 0; i < size; ++i) {
        std::cout << i << ":";
        for (auto a : adj[i]) {
            std::cout << "-> " << a;
        }
        std::cout << std::endl;
    }
}

void Graph::create_dot(const char *name, const char *filename) {
    std::stringstream dot_stream;
    const float color_step = 1.0f / size;
    // Header
    dot_stream << "strict graph " << name << " {" << std::endl;
    // Vertex colors
    for(int i = 0; i < size; ++i) {
        dot_stream << "\t" << i << " [style=\"filled\"; fillcolor=\"" << (colors[i] * color_step) << " 1.0 1.0\"]" << std::endl;
    }
    // Edges
    for(int i = 0; i < size; ++i) {
        for(auto a: adj[i]){
            dot_stream << "\t" << i << " -- " << a << std::endl;
        }
    }
    // Closing brace
    dot_stream << "}" << std::endl;

    // Output
    if(filename) {
        // File
        std::ofstream file;
        file.open(filename);
        file << dot_stream.str();
        file.close();
    }
    else {
        std::cout << dot_stream.str();
    }
}

void Graph::kcolor(int k) {
    // Alloc and init colors to 0 (unassigned)
    if(k >= size) {
        // Every vertex can have its own color
        for(int i = 0; i < size; ++i) {
            colors[i] = i;
        }
        this->colors_used = size;
        return;
    }
    this->colors_used = k;
}
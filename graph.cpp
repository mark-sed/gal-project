#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <regex>
#include "graph.hpp"

Graph::Graph(int size) : colors_used{size}, size{size} {
    this->adj = new std::vector<int>[size];
    this->colors = new Color[size]();
}

Graph::Graph(const char *filename) {
    // Sice we don't know the graph size before reading the file, a vector has to be used
    std::vector<int> f_verts_src;
    std::vector<int> f_verts_dst;

    // Read from input file
    std::ifstream file;
    file.open(filename);
    // Regex for 2 edges
    const std::regex edge_regex("[ \t]*([0-9]+)[ \t]*--[ \t]*([0-9]+).*");
    std::smatch edge_match;
    std::string line;
    int max_vertex = -1;
    // Read header
    std::getline(file, line);
    while (std::getline(file, line)) {
        if(line == "}") {
            break;
        }
        if (std::regex_match(line, edge_match, edge_regex)) {
            if(edge_match.size() < 2) {
                std::cerr << "ERROR: Incorrect input format";
                std::exit(1);
            }
            std::ssub_match sub_match = edge_match[1];
            int v1 = atoi(sub_match.str().c_str());
            f_verts_src.push_back(v1);

            std::ssub_match sub_match1 = edge_match[2];
            int v2 = atoi(sub_match1.str().c_str());
            f_verts_dst.push_back(v2);

            std::cout << v1 << " -- " << v2 << std::endl;

            if(v1 > max_vertex) {
                max_vertex = v1;
            }
            if(v2 > max_vertex) {
                max_vertex = v2;
            }   
        }   
    }
    file.close();

    // Create edges
    size = max_vertex + 1;
    this->adj = new std::vector<int>[size];
    this->colors = new Color[size]();

    for(size_t i = 0; i < f_verts_src.size(); ++i) {
        add_edge(f_verts_src[i], f_verts_dst[i]);
    }
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
    const float color_step = 1.0f / colors_used;
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

void Graph::kcolor_greedy(int k) {
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

    // Set all but 1st color as unassigned
    colors[0] = 0;
    std::fill_n(colors, size, -1);

    for(int node = 0; node < size; ++node) {
        // Cycle through colors and picke smallest one not use by neighbours
        for(int c = 0; c < k; ++c) {
            bool can_be_used = true;
            for(auto neighbour: adj[node]) {
                if(colors[neighbour] == c) {
                    can_be_used = false;
                    break;
                }
            }
            if(can_be_used) {
                colors[node] = c;
                break;
            }
            else if(c == k-1) {
                std::cerr << "Graph could not be colored" << std::endl;
                return;
            }
        }
    }
}
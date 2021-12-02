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

Graph::Graph(const char *graph_file, const char *constraint_file) {
    LOG("Loading graph");
    // Sice we don't know the graph size before reading the file, a vector has to be used
    std::vector<int> f_verts_src;
    std::vector<int> f_verts_dst;

    // Read from input file
    std::ifstream file;
    file.open(graph_file);
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
            if(edge_match.size() < 3) {
                std::cerr << "ERROR: Incorrect input format\n";
                std::exit(1);
            }
            std::ssub_match sub_match = edge_match[1];
            int v1 = atoi(sub_match.str().c_str());
            f_verts_src.push_back(v1);

            std::ssub_match sub_match1 = edge_match[2];
            int v2 = atoi(sub_match1.str().c_str());
            f_verts_dst.push_back(v2);

            if(v1 > max_vertex) {
                max_vertex = v1;
            }
            if(v2 > max_vertex) {
                max_vertex = v2;
            }   
        }
        else {
            std::cerr << "ERROR: Incorrect input format\n";
            std::exit(1);
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

    this->constraint = new std::vector<int>[size];
    // Parse constraints
    if(constraint_file != nullptr) {
        LOG(std::string("Loading constraint ") + constraint_file);
        // Read from input file
        std::ifstream con_file;
        con_file.open(constraint_file);
        // Regex for 2 edges
        const std::regex constraint_regex("[ \t]*([0-9]+)[ \t]*:[ \t]*([0-9]+).*");
        const std::regex just_vert_regex("[ \t]*([0-9]+)[ \t]*:[ \t]*");
        std::smatch constraint_match;
        // Read header
        std::getline(con_file, line);
        while (std::getline(con_file, line)) {
            if(line == "}") {
                break;
            }
            if (std::regex_match(line, constraint_match, constraint_regex)) {
                if(constraint_match.size() < 3) {
                    std::cerr << "ERROR: Incorrect input format\n";
                    std::exit(1);
                }
                std::ssub_match sub_match = constraint_match[1];
                int v1 = atoi(sub_match.str().c_str());

                std::ssub_match sub_match1 = constraint_match[2];
                int k = atoi(sub_match1.str().c_str());

                if(v1 >= size) {
                    std::cerr << "ERROR: Constraint for nonexistent vertex. Biggest vertex index is " << size-1
                              << ", but constraint is for vertex " << v1 << std::endl;
                    std::exit(1);
                }
                constraint[v1].push_back(k);
            } 
            else if(std::regex_match(line, constraint_match, just_vert_regex)) {
                if(constraint_match.size() < 2) {
                    std::cerr << "ERROR: Incorrect input format\n";
                    std::exit(1);
                }
                // This can be otherwise ignored since 
            } 
            else {
                std::cerr << "ERROR: Incorrect input format\n";
                std::exit(1);
            }
        }
        con_file.close();
    }
    LOG("Graph loaded");
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
    LOG("Creating dot");
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
    // Adding color legend
    dot_stream << "\tsubgraph clusterlegend {\n\t\tlabel=\"Color legend\";\n\t\tshape=rectangle\n";
    for(int c = colors_used-1; c >= 0; --c) {
        dot_stream << "\t\t\"Color " << c << "\" [style=\"filled\"; fillcolor=\"" << (c * color_step) << " 1.0 1.0\"]" << std::endl;
    }
    // Closing brace
    dot_stream << "\t}\n}" << std::endl;

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
    LOG("Dot created");
}

bool Graph::kcolor_greedy(int k) {
    LOG("Greedy algorithm started");
    // Alloc and init colors to 0 (unassigned)
    if(k >= size) {
        // Every vertex can have its own color
        for(int i = 0; i < size; ++i) {
            colors[i] = i;
        }
        this->colors_used = size;
        return true;
    }
    this->colors_used = k;
    // TODO set colors used

    // Reset colors
    std::fill_n(colors, size, -1);
    bool done = false;
    while(!done) {
        bool constr_fulfilled = true;
        /// First color the constrainted nodes
        for(int node = 0; node < size; ++node) {
            for(auto c: constraint[node]) {
                bool can_be_used = true;
                for(auto neighbour: adj[node]) {
                    if(colors[neighbour] == c) {
                        can_be_used = false;
                        break;
                    }
                }
                if(can_be_used) {
                    LOG(std::string("\tApplying constraint, vertex ")+std::to_string(node)+" colored "+std::to_string(c));
                    colors[node] = c;
                    break;
                }
                else if(c == k-1) {
                    // Try another coloring
                    constr_fulfilled = true;
                    break;
                }
                else {
                    LOG(std::string("\tCould not fulfill constraint for ")+std::to_string(node));
                    
                    // TODO: Find least colliding color for this, color this and 
                    // remove color for neighbours with the same one
                    

                    break;
                }
            }
        }
        if(constr_fulfilled) {
            for(int node = 0; node < size; ++node) {
                if(colors[node] >= 0 && node != size - 1) {
                    continue;
                }
                else if(colors[node] < 0 && node == size - 1) {
                    done = true; 
                    break;
                }
                // Cycle through colors and picke smallest one not use by neighbours
                for(int c = 0; c < k; ++c) {
                    bool can_be_used = true;
                    for(auto neighbour: adj[node]) {
                        if(colors[neighbour] == c 
                        || std::find(constraint[node].begin(), constraint[node].end(), c) != constraint[node].end()) {
                            can_be_used = false;
                            break;
                        }
                    }
                    if(can_be_used) {
                        colors[node] = c;
                        break;
                    }
                    else if(c == k-1) {
                        // TODO: 
                    }
                }
                if(node == size - 1) {
                    // All nodes are colored
                    done = true;
                }
            }
        }
    }
    return true;
}
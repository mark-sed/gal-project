/**
 * @file graph.cpp
 * @author Marek Sedlacek (xsedla1b@fit.vutbr.cz)
 * @brief Graph unit
 * @date November 2021
 */

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <regex>
#include "graph.hpp"
#include "gp.hpp"

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
    // Print constraints
    dot_stream << "\t\t\"Constraints:\n";
    for(int i = 0; i < size; ++i) {
        bool first = true;
        for(auto c: constraint[i]) {
            if(first) {
                dot_stream << i << ": ";
                dot_stream << c;
                first = false;
            }
            else
                dot_stream << "," << c; 
        }
        if(!first)
            dot_stream << "\\l";
    }
    dot_stream << "\" [shape=box]\n";
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

bool Graph::correctness_dfs_visit(int v, bool **visited, Color *coloring) {
    (*visited)[v] = true;
    // Check if the chosen color is in the constraint
    if(constraint[v].size() > 0 
       && std::find(constraint[v].begin(), constraint[v].end(), coloring[v]) == constraint[v].end()) {
        return false;
    }
    for(auto u: adj[v]) {
        // Check if neighbouts have different colors
        if(coloring[u] == coloring[v] || coloring[u] < 0) {
            return false;
        }
        if(!(*visited)[u]) {
            if(!correctness_dfs_visit(u, visited, coloring)){
                return false;
            }
        }
    }
    return true;
}

bool Graph::is_correctly_colored(Color *coloring) {
    coloring = (coloring ? coloring : this->colors);
    auto visited = new bool[size]();
    bool rval = true;
    for(int i = 0; i < size; ++i) {
        if(!visited[i]) {
            if(!correctness_dfs_visit(i, &visited, coloring)){
                rval = false;
                break;
            }
        }
    }
    
    delete[] visited;
    return rval;
}

bool Graph::kcolor_gp(int k, size_t popul_size, int graph_logging_period) {
    LOG("Genetic programming algorithm started");
    // Enough colors for each vertex to have unique color
    if(k >= size) {
        // Every vertex can have its own color
        for(int i = 0; i < size; ++i) {
            colors[i] = i;
        }
        this->colors_used = size;
        return true;
    }
    this->colors_used = k;

    GP::Population population(this, popul_size, k, 0.1f, 0.75f, true);
    bool done = false;
    int iteration = 0;
    while(!done) {
        ++iteration;
        LOG(std::string("Starting iteration ")+std::to_string(iteration));
        auto coloring = population.evaluate();
        if(coloring) {
            // Correct coloring found
            LOG("Found correct coloring");
            done = true;
            std::copy(coloring, coloring+size, this->colors);
        }
        // Crossover
        population.crossover();
        // Mutate
        population.mutate();

        LOG(std::string("\tBest fintess: ")+std::to_string(population.quality[0]));
        if(graph_logging_period != -1 && iteration % graph_logging_period == 0) {
            std::copy(population.candidates->front()->colors, population.candidates->front()->colors+size, this->colors);
            create_dot("partially_evolved", ("iteration"+std::to_string(iteration)+"_fit_"+std::to_string(population.quality[0])+".colored.dot").c_str());
        }
    }

    return true;
}

bool Graph::kcolor_greedy(int k) {

    // TODO
    // Use color matrix like in lectures?


    LOG("Greedy algorithm started");
    // Enough colors for each vertex to have unique color
    if(k >= size) {
        // Every vertex can have its own color
        for(int i = 0; i < size; ++i) {
            colors[i] = i;
        }
        this->colors_used = size;
        return true;
    }
    this->colors_used = k;

    // Reset colors
    std::fill_n(colors, size, -1);
    bool done = false;
    while(!done) {
        bool constr_fulfilled = true;
        /// First color the constrainted nodes
        for(int node = 0; node < size; ++node) {
            if(colors[node] >= 0) {
                continue;
            }
            for(size_t ci = 0; ci < constraint[node].size(); ++ci) {
                auto c = constraint[node][ci];
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
                else if(ci == constraint[node].size()-1) {
                    // Try another coloring
                    LOG(std::string("\tConstraint coloring has to be redone, cannot fulfill vertex ")+
                        std::to_string(node)+" constraints");
                    // Find least colliding color for this, color this and remove color for neighbours with the same one
                    int best_c = constraint[node][0];
                    int best_uses = -1;
                    for(auto poss_c: constraint[node]) {
                        int uses = 0;
                        for(auto n: adj[node]) {
                            if(std::find(constraint[n].begin(), constraint[n].end(), poss_c) != constraint[n].end()){
                                ++uses;
                            }
                        }
                        if(uses < best_uses || best_uses == -1) {
                            best_uses = uses;
                            best_c = poss_c;
                        }
                    }
                    LOG(std::string("Attempt to recolor node ")+std::to_string(node)+" using least used neighbour color "
                        +std::to_string(best_c)+" used by "+std::to_string(best_uses)+" neighbours");
                    // Color this node
                    colors[node] = best_c;
                    // Remove color from neighbours using it
                    for(auto n: adj[node]) {
                        if(colors[n] == best_c) {
                            colors[n] = -1;
                            LOG(std::string("\tUncoloring node ")+std::to_string(n));
                        }
                    }
                    constr_fulfilled = false;
                    break;
                }
                else {
                    LOG(std::string("\tCould not fulfill constraint for ")+std::to_string(node));
                }
            }
        }
        
        if(constr_fulfilled) {
            bool redo_coloring = false;
            for(int node = 0; node < size; ++node) {
                if(colors[node] >= 0 && node != size - 1) {
                    continue;
                }
                else if(colors[node] >= 0 && node == size - 1) {
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
                        // Try another coloring
                        LOG(std::string("\tColoring has to be redone, coloring for ")+
                            std::to_string(node)+" collides");

                        // Find the color least used by neighbours
                        int best_uses = -1;
                        int best_c = 0;
                        for(int n_c = 0; n_c < k; ++n_c) {
                            int uses = 0;
                            for(auto n: adj[node]) {
                                if(colors[n] == n_c) {
                                    ++uses;
                                }
                            }
                            if(uses < best_uses || best_uses == -1) {
                                best_uses = uses;
                                best_c = n_c;
                            }
                        }
                        // Uncolor neighbours and color this node
                        colors[node] = best_c;
                        for(auto n: adj[node]) {
                            if(colors[n] == best_c) {
                                colors[n] = -1;
                                LOG(std::string("\tUncoloring node ")+std::to_string(n));
                            }
                        }
                        redo_coloring = true;
                        break;
                    }
                }
                if(redo_coloring) {
                    break;
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

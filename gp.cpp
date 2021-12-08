/**
 * @file gp.cpp
 * @author Marek Sedlacek (xsedla1b@fit.vutbr.cz)
 * @brief Genetic programming unit
 * @date December 2021
 */

#include "gp.hpp"
#include <random>
#include <algorithm>
#include <iostream>

using namespace GP;

Phenotype::Phenotype(Graph *graph, int k) : graph{graph}, k{k} {
    colors = new Color[graph->size];
    for(int i = 0; i < graph->size; ++i) {
        colors[i] = rand_int(0, k-1);
    }
}

void Phenotype::fitness_dfs_visit(int v, bool **visited, int *incorrect) {
    (*visited)[v] = true;
    // Check if the chosen color is in the constraint
    if(graph->constraint[v].size() > 0 
       && std::find(graph->constraint[v].begin(), graph->constraint[v].end(), colors[v]) == graph->constraint[v].end()){
        (*incorrect)++;
    }
    for(auto u: graph->adj[v]) {
        // Check if neighbours have different colors
        if(colors[u] == colors[v] || colors[u] < 0) {
            (*incorrect)++;
        }
        if(!(*visited)[u]) {
            fitness_dfs_visit(u, visited, incorrect);
        }
    }
}

int Phenotype::fitness() {
    auto visited = new bool[graph->size]();
    int incorrect = 0;
    for(int i = 0; i < graph->size; ++i) {
        if(!visited[i]) {
            fitness_dfs_visit(i, &visited, &incorrect);
        }
    }
    delete[] visited;
    return incorrect;
}

Population::Population(Graph *graph, size_t size, int k) : graph{graph}, size{size}, k{k} {
    this->candidates = new std::list<Phenotype *>();
    for(size_t i = 0; i < size; ++i) {
        this->candidates->push_back(new Phenotype(graph, k));
    }
    this->quality = new int[size];
}

Population::~Population() {
    for(auto *c: *this->candidates) {
        delete c;
    }
    delete this->candidates;
    delete[] quality;
}

Color *Population::evaluate() {
    int i = 0;
    for(auto *pheno: *this->candidates) {
        quality[i] = pheno->fitness();
        std::cout << quality[0] << std::endl;
        if(quality[i] == 0) {
            // Correct coloring found
            std::cout << "AAAA\n" << i << "," << quality[i] << "\n";
            return pheno->colors;
        }
        ++i;
    }
    return nullptr;
}
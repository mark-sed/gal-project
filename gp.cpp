/**
 * @file gp.cpp
 * @author Marek Sedlacek (xsedla1b@fit.vutbr.cz)
 * @brief Genetic programming unit
 * @date December 2021
 */

#include "gp.hpp"
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace GP;

Phenotype::Phenotype(Graph *graph, int k) : graph{graph}, k{k} {
    colors = new Color[graph->size];
    this->mutatable = new std::vector<int>;
    for(int i = 0; i < graph->size; ++i) {
        colors[i] = rand_int(0, k-1);
    }
}

Phenotype::Phenotype(Graph *graph, int k, bool correct_colors) : graph{graph}, k{k} {
    colors = new Color[graph->size];
    this->mutatable = new std::vector<int>;
    // Pick a random color but from constraint list or one fulfilling the coloring 
    for(int i = 0; i < graph->size; ++i) {
        if(!graph->constraint[i].empty()) {
            colors[i] = graph->constraint[i][rand_int(0, graph->constraint[i].size()-1)];
        }
        else {
            bool colored = false;
            for(int c = 0; c < k; ++c) {
                bool used = false;
                for(auto n: graph->adj[i]) {
                    if(colors[n] == c) {
                        used = true;
                        break;
                    }
                }
                if(!used) {
                    colors[i] = c;
                    colored = true;
                    break;
                }
            }
            if(!colored) {
                // If coloring cannot be fulfilled, then pick a random color
                colors[i] = rand_int(0, k-1);
            }
        }
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
            this->mutatable->push_back(u);
        }
        if(!(*visited)[u]) {
            fitness_dfs_visit(u, visited, incorrect);
        }
    }
}

int Phenotype::fitness() {
    auto visited = new bool[graph->size]();
    this->mutatable->clear();
    int incorrect = 0;
    for(int i = 0; i < graph->size; ++i) {
        if(!visited[i]) {
            fitness_dfs_visit(i, &visited, &incorrect);
        }
    }
    delete[] visited;
    return incorrect;
}

void Phenotype::mutate() {
    int rand_node = rand_int(0, graph->size-1);
    int rand_color = rand_int(0, k-1);
    colors[rand_node] = rand_color;
}

void Phenotype::mutate_heuristic() {
    // Only mutate incorrect nodes
    if(mutatable->empty())
        return;
    int rand_node = (*this->mutatable)[rand_int(0, this->mutatable->size()-1)];
    int rand_color;
    if(graph->constraint[rand_node].empty())
        rand_color = rand_int(0, k-1);
    else
        rand_color = graph->constraint[rand_node][rand_int(0, graph->constraint[rand_node].size()-1)];
    colors[rand_node] = rand_color;
}

void Phenotype::crossover(const Phenotype *other) { 
    int rand1 = rand_int(0, graph->size-1);
    int rand2 = rand_int(0, graph->size-1);
    int start = rand1 < rand2 ? rand1 : rand2;
    int end = rand1 < rand2 ? rand2 : rand1;
    std::copy(other->colors + start, other->colors + end, colors+start);
}

Population::Population(Graph *graph, size_t size, int k,
                       float mutate_chance, float crossover_chance, bool elitism,
                       bool correct_phenos) 
                       : graph{graph}, size{size}, k{k}, 
                         mutate_chance{mutate_chance}, crossover_chance{crossover_chance}, elitism{elitism} {
    this->candidates = new std::list<Phenotype *>();
    for(size_t i = 0; i < size; ++i) {
        if(correct_phenos){
            this->candidates->push_back(new Phenotype(graph, k, true));
        }
        else {
            this->candidates->push_back(new Phenotype(graph, k));
        }
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
    int best_i = -1;
    std::list<Phenotype *>::iterator best;
    for(auto pheno = this->candidates->begin(); pheno != this->candidates->end(); ++pheno) {
        quality[i] = (*pheno)->fitness();
        if(quality[i] == 0) {
            // Correct coloring found
            return (*pheno)->colors;
        }
        if(best_i == -1 || quality[i] < quality[best_i]) {
            best_i = i;
            best = pheno;
        }
        ++i;
    }
    // Put the best pheno at the begining
    std::swap(quality[0], quality[best_i]);
    std::swap(*best, *(this->candidates->begin()));
    return nullptr;
}

void Population::mutate() {
    int amount_done = 0;
    for(auto *pheno: *candidates) {
        if(rand_float() <= mutate_chance) {
            if(elitism && pheno == candidates->front()) {
                continue;
            }
            pheno->mutate();
            ++amount_done;
        }
    }
    LOG(std::to_string(amount_done)+" mutations done");
}

void Population::mutate_heuristic() {
    for(auto *pheno: *candidates) {
        pheno->mutate_heuristic();
    }
    LOG("All phenotype heuristic mutations done");
}

void Population::crossover() {
    size_t size = candidates->size();
    int amount_done = 0;
    for(auto pheno = this->candidates->begin(); pheno != this->candidates->end(); ++pheno) {
        if(elitism && *pheno == candidates->front()) {
            continue;
        }
        if(rand_float() <= crossover_chance) {
            int offset;
            Phenotype *other;
            do {
                offset = rand_int(0, size-1);
                auto it = this->candidates->begin();
                std::advance(it, offset);
                other = *it;
            } while(other == *pheno);
            (*pheno)->crossover(other);
            ++amount_done;
        }
    }
    LOG(std::to_string(amount_done)+" crossovers done");
}

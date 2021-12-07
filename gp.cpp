/**
 * @file gp.cpp
 * @author Marek Sedlacek (xsedla1b@fit.vutbr.cz)
 * @brief Genetic programming unit
 * @date December 2021
 */

#include "gp.hpp"

using namespace GP;

Population::Population(size_t size) : size{size} {
    this->candidates = new std::list<Phenotype *>();
}

Population::~Population() {
    for(auto *c: *this->candidates) {
        delete c;
    }
    delete this->candidates;
}
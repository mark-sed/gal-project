/**
 * @file gp.hpp
 * @author Marek Sedlacek (xsedla1b@fit.vutbr.cz)
 * @brief Genetic programming unit
 * @date December 2021
 */

#ifndef _GP_HPP_
#define _GP_HPP_

#include "graph.hpp"
#include <list>
#include <stddef.h>

/** Genetic programming resources */
namespace GP {
    
    /** Phenotype of a genome */
    class Phenotype {
    private:
        Graph *graph;  ///< The main graph for this phenotype
    public:
        Color *colors; ///< Phenotype's coloring
        
        /**
         * @brief Construct a new Phenotype object
         * @param graph Graph in which this phenotype is evolving
         */
        Phenotype(Graph *graph) : graph{graph} {
            colors = new Color[graph->size];
        }
        /**
         * @brief Destroy the Phenotype object
         */
        ~Phenotype() {
            delete[] colors;
        }
    };


    /** Population of phenotypes for genetic programming */
    class Population { 
    private:
        size_t size;  ///< Population size
        std::list<Phenotype *> *candidates;  ///< Candidate phenotypes
    public:
        /**
         * @brief Construct a new Population object
         * @param size The amount of candidate colorings (phenotypes) to create
         */
        Population(size_t size);
        /**
         * @brief Destroy the Population object
         */
        ~Population();
    };
};

#endif//_GP_HPP_
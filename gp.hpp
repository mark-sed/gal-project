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
#include <vector>
#include <stddef.h>
#include <random>
#include <time.h>
#include <cstdlib>

/** Genetic programming resources */
namespace GP {
    
    inline void init() {
        std::srand(time(nullptr));
    }

    inline int cpprand_int(int min, int max) {
        return (std::rand() % (max - min + 1)) + min;
    }

    inline float rand_float() {
        return static_cast<float>(rand()) / RAND_MAX;
    }

    inline int fastrand_int(int min, int max) { 
        static unsigned int g_seed = time(nullptr);
        g_seed = (214013*g_seed+2531011); 
        auto r = (g_seed>>16)&0x7FFF; 
        return (r % (max - min + 1)) + min;
    } 

    inline int rand_int(int min, int max) {
        return fastrand_int(min, max);
    }

    /** Phenotype of a genome */
    class Phenotype {
    private:
        Graph *graph;  ///< The main graph for this phenotype
        int k;
        std::vector<int> *mutatable; ///< Vector of nodes that can be mutated when using heuristics

        /**
         * DFS visit for fitness calculation
         * @param v Node to visit
         * @param visited Array of visted flags
         * @param[out] incorrect The amount of incorrectly colored nodes
         */
        void fitness_dfs_visit(int v, bool **visited, int *incorrect);
    public:
        Color *colors; ///< Phenotype's coloring
        
        /**
         * @brief Construct a new Phenotype object
         * @param graph Graph in which this phenotype is evolving
         * @param k The amount of colors to color with
         */
        Phenotype(Graph *graph, int k);

        /**
         * @brief Construct a new Phenotype object
         * This constructor creates phenotypes with partially correct coloring
         * @param graph Graph in which this phenotype is evolving
         * @param k The amount of colors to color with
         * @param correct_colors 
         */
        Phenotype(Graph *graph, int k, bool correct_colors);


        /**
         * @brief Destroy the Phenotype object
         */
        ~Phenotype() {
            delete[] colors;
            delete mutatable;
        }

        /**
         * Calculates fitness value for a phenotype
         * @return Phenotype's fitness where the lower it is the better and 0 is correctly colored graph
         */
        int fitness();

        /** Mutates the phenotype */
        void mutate();

        /** Mutates the phenotype using heuristics */
        void mutate_heuristic();

        /** 
         * Crosses over this phenotype with passed in one
         * @param other Other phenotype to crossover with
         */ 
        void crossover(const Phenotype *other);
    };


    /** Population of phenotypes for genetic programming */
    class Population { 
    private:
        Graph *graph; ///< Graph in which this population is evolving
        size_t size;
        int k;        ///< Number of available colors (k-coloring)
        float mutate_chance;    ///< Chance of one phenotype to mutate
        float crossover_chance; ///< Chance for phenotye to crossover another one
        bool elitism;           ///< If true, then best quality phenotype won't be evolved
    public:
        std::list<Phenotype *> *candidates;  ///< Candidate phenotypes
        int *quality; ///< An array of fitness quality values for each phenotype
        
        /**
         * @brief Construct a new Population object
         * @param size The amount of candidate colorings (phenotypes) to create
         * @param k The amount of colors to color with
         * @param mutate_chance Chance of one phenotype to mutate
         * @param crossover_chance Chance for phenotye to crossover another one
         * @param elitism If true, then best quality phenotype won't be evolved
         */
        Population(Graph *graph, size_t size, int k, 
                   float mutate_chance=0.1f, float crossover_chance=0.75, bool elitism=true,
                   bool correct_phenos=false);
        /**
         * @brief Destroy the Population object
         */
        ~Population();

        /**
         * @brief Updates fitness values
         * 
         * @return Color* 
         */
        Color *evaluate();

        /** Mutates phenotypes based on set chances and evolution attributes */
        void mutate();

        /** Mutates heuristically phenotypes based on set chances and evolution attributes */
        void mutate_heuristic();

        /** Crosses over phenotypes based on set evolution attributes */ 
        void crossover();
    };
};

#endif//_GP_HPP_
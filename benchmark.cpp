/**
 * @file benchmark.cpp
 * @author Jiri Kristof (xkrist22@stud.fit.vutbr.cz)
 * @brief Benchmark unit
 * @date December 2021
 */

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <chrono>
#include <vector>
#include "benchmark.hpp"
#include "gp.hpp"
#include "graph.hpp"

 Benchmark::Benchmark() {

 }

 Benchmark::~Benchmark() {

 }

void Benchmark::run_benchmark(const char* input_file, const char* output_file) {
    // open input file for reading 
    std::ifstream input_file_handler(input_file);
    std::string bench_line;
    // regex for checking line correctness
    std::regex line_regex("^[0-9]+,[geh],[-._/A-Za-z+-9]+,[-._/A-Za-z+-9]+,[0-9]+,[0-9]+,[0-9]+$");

    // remove existing data in output file
    std::ofstream output_file_hanlder;
    output_file_hanlder.open(output_file, std::ofstream::out | std::ofstream::trunc);
    output_file_hanlder.close();

    // this serves as notificator that benchmark is not stuck
    int counter = 0;
    std::cout << "Progress: ";
    std::cout.flush();
    while (getline(input_file_handler, bench_line)) {
        // second part of progress bar
        if (counter >= COUNTER_LIMIT) {
            counter = 0;
            std::cout << "|";
            std::cout.flush();
        }
        counter++;
        // if line is not correctly written, then skip it
        if (!std::regex_search(bench_line, line_regex)) {
            continue;
        }
        // parse each line - each column value will be stored in vector element in previous order
        std::vector<std::string> input_data;        
        std::istringstream string_stream(bench_line);
        std::string column_data;
        while(std::getline(string_stream, column_data, DELIMITER)) {
            input_data.push_back(column_data);
        }

        bench_run_t options;

        // get identificator (note that value was already checked in regex)
        options.identificator = std::stoi(input_data[0]);

        // get algorithm
        options.algorithm = input_data[1][0];

        // get graph file
        options.graph_file = input_data[2].c_str();

        // get constraints file
        if (input_data[3] == "x" || input_data[3] == "X") {
            options.constraints_file = nullptr;
        } else {
            options.constraints_file = input_data[3].c_str();
        }

        // get number of colors
        options.colors = std::stoi(input_data[4]);
        // if color is set incorrectly, continue with next line
        if (options.colors <= 0) {
            continue;
        }

        // get number of population
        options.population = std::stoi(input_data[5]);
        // if population is set incorrectly for 'e' or 'h' algorithm, continue with next line
        if (options.algorithm != 'g' && options.population <= 0) {
            continue;
        }

        // get number of repetition
        options.repetition = std::stoi(input_data[6]);
        // no check performed, so benchmark can be "commented out" using 0 value

        for (int i = 0; i < options.repetition; i++) {
            if (options.algorithm == 'e') {
                break;
            }
            results_t results = bench_run(options);
            write_results(output_file, results);
        }
    }
    std::cout << std::endl;
}

Benchmark::results_t Benchmark::bench_run(bench_run_t options) {
    auto g = new Graph(options.graph_file, options.constraints_file);

    // create result structure and fill static values
    results_t results;
    // save number of vertices to result
    results.node_num = g->size;
    // save number of edges to result
    results.edge_num = 0;
    // for each node, count number of neighbors
    for(int i = 0; i < results.node_num; ++i) {
        results.edge_num += g->adj[i].size();
    }
    // unoriented graph - for 1 edge there is 2 records
    results.edge_num /= 2;
    // save number of constraints to result
    results.constraint_num = 0;
    // for each node, count number of constraints
    for(int i = 0; i < results.node_num; ++i) {
        results.constraint_num += g->constraint[i].size();
    }

    // run selected algorithm with given options
    if (options.algorithm == 'g') {
        // run algorithm and count time spended in function
        auto start = std::chrono::system_clock::now();
        g->kcolor_greedy(options.colors);
        auto end = std::chrono::system_clock::now();

        // get results of run
        std::chrono::duration<float, std::milli> elapsed = end - start;

        // set identificator of run to result
        results.identificator = options.identificator;
        // set elapsed time to result
        results.time = elapsed.count();
        // set info about correctness coloring to result
        results.success = g->is_correctly_colored();

        return results;

    } else if (options.algorithm == 'e') {
        GP::init();
        auto start = std::chrono::system_clock::now();
        g->kcolor_gp(options.colors, options.population);
        auto end = std::chrono::system_clock::now();

        // get results of run
        std::chrono::duration<float, std::milli> elapsed = end - start;
        // set identificator of run to result
        results.identificator = options.identificator;
        // set elapsed time to result
        results.time = elapsed.count();
        // set info about correctness coloring to result
        results.success = g->is_correctly_colored();

        return results;

    } else {
        GP::init();
        auto start = std::chrono::system_clock::now();
        g->kcolor_gp_heuristic(options.colors, options.population);
        auto end = std::chrono::system_clock::now();

        // get results of run
        std::chrono::duration<float, std::milli> elapsed = end - start;
        // set identificator of run to result
        results.identificator = options.identificator;
        // set elapsed time to result
        results.time = elapsed.count();
        // set info about correctness coloring to result
        results.success = g->is_correctly_colored();

        return results;
    }
}

void Benchmark::write_results(const char* output_file, results_t results) {
    // open file for appending new data
    std::ofstream output_file_hanlder;
    output_file_hanlder.open(output_file, std::ios_base::app);

    // append new data in csv format
    output_file_hanlder << results.identificator << ","
                        << results.time << "," 
                        << results.success << ","
                        << results.node_num << ","
                        << results.edge_num << ","
                        << results.constraint_num << std::endl;

    // close file after write
    output_file_hanlder.close();
}
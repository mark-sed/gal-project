/**
 * @file benchmark.hpp
 * @author Jiri Kristof (xkrist22@stud.fit.vutbr.cz)
 * @brief Benchmark unit
 * @date December 2021
 */

#ifndef _BENCHMARK_HPP_
#define _BENCHMARK_HPP_

#define DELIMITER ','

/** Graph representation using adjacency list */
class Benchmark {
public:
    /** Constructor */
    Benchmark();

    /** Destructor */
    ~Benchmark();

    /** 
     * Method runs benchmarking according to options in input file
     * and write results to output file
     * 
     * @param input_file input file describing each benchmark run, format described in README
     * @param output_file file into which output of benchmark will be written
     */
    void run_benchmark(const char* input_file, const char* output_file);

private:
    /** Options for benchmark runs */
    struct bench_run_t {
        int identificator;
        char algorithm;
        const char* graph_file;
        const char* constraints_file;
        int colors;
        int population;
        int repetition;
    };

    /** results of benchmark run */
    struct results_t {
        int identificator;
        float time;
        bool success;
        int node_num;
        int edge_num;
        int constraint_num;
        // TODO add something???
    };

    /**
     * Method runs benchmark according to given settings
     *
     * @param options structure containing options for benchmark run
     * 
     * @return results in struct
     */
    results_t bench_run(bench_run_t options);

    /**
     * Method writes results of each run into output file
     *
     * @param output_file file into which results will be written
     * @param results structure with results from benchmark
     */
    void write_results(const char* output_file, results_t results);
};

#endif // _BENCHMARK_HPP_
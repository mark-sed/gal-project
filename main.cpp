#include <iostream>
#include "graph.hpp"
#include "gp.hpp"
#include <chrono>
#include <stdio.h> 
#include <stdlib.h> 
#include <getopt.h> 
#include <ctype.h>
#include <string.h>

void print_help(const char* error_msg) {

    if (strcmp(error_msg, "")) {
        std::cout << "ERROR: " << error_msg << std::endl;
    }

    std::cout << "HELP:" << std::endl
                << "\tSelect one of the algorithms:" << std::endl
                << "\t\t--greedy\tGreedy algorithm" << std::endl
                << "\t\t--evolution\tEvolution algorithm using genetic programming" << std::endl
                << "\t\t--heuristic\tEvolution algorithm using genetic programming with heuristic" << std::endl

                << "\tDefine input file with graph:" << std::endl
                << "\t\t--graph <input graph filename>" << std::endl
                << "\t\tFormat of <input graph filename> is described in README" << std::endl

                << "\tDefine output file into which colored graph will be saved:" << std::endl
                << "\t\t--output <output filename>" << std::endl
                << "\t\tFormat of <output graph filename> is described in README" << std::endl
                << "\t\tNote that if using benchmark mode, then output is statistics, not graph" << std::endl

                << "\tSet number of colors used for graph coloring:" << std::endl
                << "\t\t--colors <int>" << std::endl

                << "\tOptionally, set file with constraints:" << std::endl
                << "\t\t--constraints <input constraints filename>" << std::endl
                << "\t\tFormat of <input constraints filename> is described in README" << std::endl

                << "\tAlternatively, run program in benchmark mode:" << std::endl
                << "\t\t--benchmark <input benchmark file>" << std::endl
                << "\t\tFormat of <input benchmark filename> is described in README" << std::endl
                << "\t\tNote that you still must define output file, where statistics in csv format will be printed" << std::endl
    ;

    exit(1);
}

int main(int argc, char *argv[]) {
    // variables for getopt
    int option;
    int option_index = 0;

    // variables for saving settings
    char algorithm = 'x';
    char* graph_file = nullptr;
    char* constraints_file = nullptr;
    char* benchmark_file = nullptr;
    char* output_file = nullptr;
    int colors = 0;

    struct option  long_options[] = {
        {"greedy", no_argument, nullptr, 'g'},
        {"evolution", no_argument, nullptr, 'e'},
        {"heuristic", no_argument, nullptr, 'u'},
        {"graph", required_argument, nullptr, 'r'},
        {"constraints", required_argument, nullptr, 'c'},
        {"benchmark", required_argument, nullptr, 'b'},
        {"output", required_argument, nullptr, 'o'},
        {"colors", required_argument, nullptr, 'l'},
        {"help", no_argument, nullptr, 'h'},
        {0, 0, 0, 0} // ending element to prevent "segmentation fault"
    };

    const char* short_options = "geur:c:b:hl:o:";

    // parse input arguments and check if combinations of them are correct
    while ((option = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch(option) {
            case 'g':
                // check if another algorithm was not selected
                if (algorithm != 'x') {
                    print_help("cannot select more than 1 algorithm");
                }
                // save selected algorithm
                algorithm = 'g';
                break;

            case 'e':
                // check if another algorithm was not selected
                if (algorithm != 'x') {
                    print_help("cannot select more than 1 algorithm");
                }
                // save selected algorithm
                algorithm = 'e';
                break;

            case 'u':
                // check if another algorithm was not selected
                if (algorithm != 'x') {
                    print_help("cannot select more than 1 algorithm");
                }
                // save selected algorithm
                algorithm = 'h';
                break;

            case 'b':
                // check if another algorithm was not selected
                if (algorithm != 'x') {
                    print_help("cannot select more than 1 algorithm");
                }
                // save selected algorithm
                algorithm = 'b';
                // save file with benchmark settings
                benchmark_file = (char*) malloc(sizeof(char) * strlen(optarg));
                strcpy(benchmark_file, optarg);
                break;

            case 'r':
                // check if file was not set already
                if (graph_file != nullptr) {
                    print_help("Graph file set multiple times");
                }
                graph_file = (char*) malloc(sizeof(char) * strlen(optarg));
                strcpy(graph_file, optarg);
                break;
            
            case 'c':
                // check if file was not set already
                if (constraints_file != nullptr) {
                    print_help("Constraints file set multiple times");
                }
                constraints_file = (char*) malloc(sizeof(char) * strlen(optarg));
                strcpy(constraints_file, optarg);
                break;

            case 'o':
                // check if file was not set already
                if (output_file != nullptr) {
                    print_help("Output file set multiple times");
                }
                output_file = (char*) malloc(sizeof(char) * strlen(optarg));
                strcpy(output_file, optarg);
                break;

            case 'l':
                // check if number of colors was not defined already
                if (colors != 0) {
                    print_help("Number of colors defined multiple times");
                } 
                // check if given value is number
                for (size_t i = 0; i < strlen(optarg); i++) {
                    if (!isdigit(optarg[i])) {
                        print_help("Number of colors must be number");
                    }
                }
                colors = atoi(optarg);
                break;

            case 'h':
                print_help("");
                break;

            case '?':
                print_help("Unknown option, described above");
                break;

            default:
                print_help("Error during argument parsing, described above");
                break;
        }
    }

    // check if settings given by arguments is correct
    if (algorithm == 'x') {
        print_help("no algorithm selected");
    } else if (algorithm == 'b') {
        if (benchmark_file == nullptr) {
            print_help("No benchmark file selected");
        } 
    } else {
        if (graph_file == nullptr) {
            print_help("No graph file selected");
        }
        if (colors == 0) {
            print_help("Number of colors not set");
        }
    }
    if (output_file == nullptr) {
        print_help("No output file selected");
    }

    // run selected algorithm
    if (algorithm == 'g') {
        auto g = new Graph(graph_file, constraints_file);
        g->kcolor_greedy(colors);
    } else if (algorithm == 'e') {
        auto g = new Graph(graph_file, constraints_file);
        GP::init();
        g->kcolor_gp(colors, 20);
    } else if (algorithm == 'h') {
        auto g = new Graph(graph_file, constraints_file);
        GP::init();
        g->kcolor_gp_heuristic(colors, 20);
    } else {
        ; // TODO benchmark
    }

    return 0;
}
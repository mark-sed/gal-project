#include <stdio.h>
#include <unistd.h>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include "src/Graph.hpp"


int main(int argc, char* argv[]) {
    // parse cmd line args
    int opt;
    std::string filename = "";

    while((opt = getopt(argc, argv, ":if:lrx")) != -1) {
        switch(opt) {
            case 'f':
                std::cout << "* Filename: " << optarg << std::endl;
                filename = optarg;
        }
    }
}
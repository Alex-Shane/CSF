#include <iostream>
#include <string> 
#include <sstream>
#include <cstring>
#include "csimfuncs.h"

int main(int argc, char** argv) {
    /*
    special combinations:
        direct mapped: argv[2] == 1 
        fully associative: argv[1]== 1
        set-associative: n argv[1] AND m argv[2]
    assumptions:
        loads/stores from/to cache take one processor cycle
        loads/stores from/to memory take 100 processor cycles per 4 bytes quantity transfered
    */ 

    // check that input parameters are valid 
    if (validParameters(argc, argv)) {
        // initalize int versions of cache parameters
        int sets = std::stoi(argv[1]);
        int blocks = std::stoi(argv[2]);
        int bytes = std::stoi(argv[3]);
        // initalize cache
        Cache cache = initializeCache(sets, blocks);
        // find # of tag bits
        int tagBits = findTagBits(blocks, bytes);
        // initalize loads and stores to zero
        int loads = 0;
        int stores = 0;
        std::string input; 
        // read entire input text 
        while (std::getline(std::cin, input)) {
            // create a stream to read 3 sections of each line
            std::istringstream stream(input);
            std::string command, address, ignore;
            // separate stream into 3 distinct sections
            stream >> command >> address >> ignore;
            if (command == "l") {
                std::cout << "load!" << std::endl;
                loads++;
            }
            else {
                std::cout << "store!" << std::endl;
                stores++;
            }
        }
        std::cout << "loads: " << loads << ", stores: " << stores << std::endl;
        return 0;
    }
    else {
        // error messages dealt with in function, just need to return non zero exit code
        return 1;
    }
}


#include <cstdint>
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
        // initalize cache parameters
        int sets = std::stoi(argv[1]);
        int blocks = std::stoi(argv[2]);
        int bytes = std::stoi(argv[3]);
        bool write_allocate = strcmp("write-allocate", argv[4]) == 0 ? true : false;
        bool write_through = strcmp("write-through", argv[5]) == 0 ? true : false;
        bool lru = strcmp(argv[6], "lru") == 0 ? true : false;
        // initalize cache
        Cache cache = initializeCache(sets, blocks);
        // initalize simulation counters to zero
        int loads = 0;
        int stores = 0;
        int load_hits = 0;
        int load_misses = 0;
        int store_hits = 0;
        int store_misses = 0;
        int total_cycles = 0;
        std::string input; 
        // read entire input text 
        while (std::getline(std::cin, input)) {
            // create a stream to read 3 sections of each line
            std::istringstream stream(input);
            std::string command, address_str, ignore;
            uint32_t address;
            // separate stream into 3 distinct sections
            stream >> command >> address_str >> ignore;
            // address converted from hex
            address = std::stoul(address_str, nullptr, 16);
            // find tag 
            uint32_t tag = getTag(bytes, address, sets);
            // find index
            uint32_t index = getIndex(bytes, address, sets);
            if (command == "l") {
                int cycles = cacheLoad(cache, index, tag, bytes, write_through, lru);
                if (cycles == 1) {
                    load_hits++;
                } 
                else {
                    load_misses++;
                }
                loads++;
                total_cycles += cycles;
            }
            else {
                bool hit = false; 
                int cycles = cacheStore(cache, index, tag, bytes, write_allocate, write_through, lru, &hit);
                if (hit) {
                    store_hits++;
                }
                else {
                    store_misses++;
                }
                stores++;
                total_cycles += cycles;
            }
        }
        std::cout << "Total loads: " << loads << std::endl;
        std::cout << "Total stores: " << stores << std::endl;
        std::cout << "Load hits: " << load_hits << std::endl;
        std::cout << "Load misses: " << load_misses << std::endl;
        std::cout << "Store hits: " << store_hits << std::endl;
        std::cout << "Store misses: " << store_misses << std::endl;
        std::cout << "Total cycles: " << total_cycles << std::endl;
        return 0;
    }
    else {
        // error messages dealt with in valid parameters function, just need to return non zero exit code
        return 1;
    }
}


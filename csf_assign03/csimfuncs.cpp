#include <iostream>
#include <string> 
#include <sstream>
#include <cstring>
#include "csimfuncs.h"
#include <cmath>

bool checkPowerOfTwo(int num) {
    // power of 2 only has one bit set so if we check bits of num
    // and the num one smaller than it, no bits should line up
    return (num & (num-1)) == 0;
}

bool isSetAndBlockValid(char* set, char* block) {
    try {
        int sets = std::stoi(set);
        int blocks = std::stoi(block);
        if (sets <= 0 || blocks <= 0) {
            return false;
        }
        return (checkPowerOfTwo(sets) && checkPowerOfTwo(blocks));
    }
    catch (std::invalid_argument& e) {
        return false;
    }
}

bool isByteValid(char *byte) {
    try {
        int bytes = std::stoi(byte);
        if (bytes < 4) {
            return false;
        }
        return checkPowerOfTwo(bytes);
    }
    catch (std::invalid_argument& e) {
        return false;
    }
}

bool isValidOption(char* input, char* option1, char* option2) {
    int checkOption1 = strcmp(input, option1);
    int checkOption2 = strcmp(input, option2);
    if (checkOption1 == 0 || checkOption2 == 0) {
        return true;
    }
    return false;
}

bool validParameters(int argc, char** argv) {
    // check for valid number of arguments
    if (argc != 7) {
        std::cerr << "You must enter 7 and only 7 parameters. Please try again" << std::endl;
        return false;
    }
    // check set and block validity
    if (!isSetAndBlockValid(argv[1], argv[2])) {
        std::cerr << "Please enter a positive power of 2 for number of sets or blocks in cache" << std::endl;
        return false;
    }
    // check byte validity
    if (!isByteValid(argv[3])) {
        std::cerr << "Please enter a postive power of 2 greater than or equal to 4 for number of bytes per block" << std::endl;
        return false;
    }
    // check that argv[4] is either w.a or n.w.a
    if (!isValidOption(argv[4], (char*)"write-allocate", (char*)"no-write-allocate")) {
        std::cerr << "Please enter either write-allocate or no-write-allocate for the 5th parameter" << std::endl;
        return false;
    }
    // check that argv[5] is either w.t or w.b
    if (!isValidOption(argv[5], (char*)"write-through", (char*)"write-back")) {
        std::cerr << "Please enter either write-through or write-back for the 6th parameter" << std::endl;
        return false;
    }
    // check that argv[6] is either lru or fifo
    if (!isValidOption(argv[6], (char*)"lru", (char*)"fifo")) {
        std::cerr << "Please enter either lru or fifo for the 7th parameter" << std::endl;
        return false;
    }
    return true;
}

int findTagBits(int blocks, int bytes) {
    // get the offset and index bits
    int offsetBits = log(bytes) / log(2);
    int indexBits = log(blocks) / log(2);
    // calculate the tag bits from offset and index bits
    int tagBits = 32-(offsetBits+indexBits);
    return tagBits;
}

// Function to initialize an empty cache
Cache initializeCache(int numSets, int numSlotsPerSet) {
    Cache cache;
    // allocate proper memory for cache
    cache.sets.resize(numSets);
    // create the neccessary sets for the cache
    for (int i = 0; i < numSets; ++i) {
        Set set;
        // allocate proper memory for set
        set.slots.resize(numSlotsPerSet); 
        // create the neccessary slots for current set 
        for (int j = 0; j < numSlotsPerSet; ++j) {
            Slot slot;
            // slot is initially not valid
            slot.valid = false;
            // slot intially has zero loads or stores
            slot.load_ts = 0;
            slot.access_ts = 0;
            // add slot to current set
            set.slots[j] = slot; 
        }
        // add current set to cache
        cache.sets[i] = set;
    }
    return cache;
}
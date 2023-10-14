#include <cstdint>
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

uint32_t getTag(int blocks, int bytes, uint32_t address) {
    // get the offset and index bits
    int offsetBits = log(bytes) / log(2);
    int indexBits = log(blocks) / log(2);
    // get the tag by getting the remaining bits of the address not made up of offset or index
    uint32_t tag = address >> (offsetBits + indexBits);
    return tag;
}

uint32_t getIndex(int blocks, int bytes, uint32_t address) {
    // get the offset and index bits
    int offsetBits = log(bytes) / log(2);
    int indexBits = log(blocks) / log(2);
    // make every digit of index a 1
    uint32_t indexMask = ((1 << indexBits) - 1) << offsetBits;
    // use bitwise AND to get correct digits for index and shift it right to bring index to least significant places 
    uint32_t index = (address & indexMask) >> offsetBits;
    return index;
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
            slot.store_ts = 0;
            // add slot to current set
            set.slots[j] = slot; 
        }
        // add current set to cache
        cache.sets[i] = set;
    }
    if (numSlotsPerSet == 1) {
        cache.directMapped = true;
        cache.fully_associative = false;
    } 
    else if (numSets == 1) {
        cache.fully_associative = true;
        cache.directMapped = false;
    } 
    else {
        cache.directMapped = false;
        cache.fully_associative = false;
    }
    return cache;
}

// simulate a cache load, where the return int is the number of cycles the load took 
int cacheLoad(Cache& cache, uint32_t index, uint32_t tag, int data_size) {
    Set& cacheSet = cache.sets[index];
    // Search for a matching tag in the cache set
    for (Slot& slot : cacheSet.slots) {
        if (slot.valid && slot.tag == tag) {
            // Cache hit
            slot.load_ts++;
            return 1;
        }
        // even if slot isn't a match, update its timestamp
        slot.load_ts++;
    } 
    // Determine which cache slot to use for miss 
    int slotToReplace;
    if (cache.directMapped) {
        slotToReplace = 0;
    }
    // Replace the cache slot with the new data
    Slot& replacementSlot = cacheSet.slots[slotToReplace];
    replacementSlot.tag = tag;
    replacementSlot.valid = true;
    // load miss so we have 100 cycles per 4 bytes we had to load from main memory, which is 25 cycle/byte, so 25*bytes loaded
    return 25*data_size;
}

// simulate a cache store, where the return int is the number of cycles the store took 
int cacheStore(Cache& cache, uint32_t index, uint32_t tag, int data_size) {
    Set& cacheSet = cache.sets[index];
    // Search for a matching tag in the cache set
    for (Slot& slot : cacheSet.slots) {
        if (slot.valid && slot.tag == tag) {
            // Cache hit
            slot.store_ts++;
            return 1;
        }
        // even if slot isn't a match, update its timestamp
        slot.store_ts++;
    } 
    // Determine which cache slot to use for miss 
    int slotToReplace;
    if (cache.directMapped) {
        slotToReplace = 0;
    }
    // Replace the cache slot with the new data
    Slot& replacementSlot = cacheSet.slots[slotToReplace];
    replacementSlot.tag = tag;
    replacementSlot.valid = true;
    // load miss so we have 100 cycles per 4 bytes we had to load from main memory, which is 25 cycle/byte, so 25*bytes loaded
    return 25*data_size;
}
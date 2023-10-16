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

uint32_t getTag(int blocks, int bytes, uint32_t address, int sets) {
    // get the offset and index bits
    int offsetBits = log(bytes) / log(2);
    // if full associative cache, we want tag to be tag+index bits
    if (sets == 1) {
        uint32_t tag = address >> (offsetBits);
        return tag;
    }
    // if direct mapped or set associative, proceed normally with index and tag
    int indexBits = log(blocks) / log(2);
    // get the tag by getting the remaining bits of the address not made up of offset or index
    uint32_t tag = address >> (offsetBits + indexBits);
    return tag;
}

uint32_t getIndex(int blocks, int bytes, uint32_t address, int sets) {
    // index should be zero every time if fully associative cache
    if (sets == 1) {
        return 0;
    }
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
            // slot is initially not valid or dirty
            slot.valid = false;
            slot.dirty = false;
            // slot intially has zero loads or stores
            slot.load_ts = 0;
            slot.access_ts = 0;
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
int cacheLoad(Cache& cache, uint32_t index, uint32_t tag, int data_size, bool lru) {
    Set& cacheSet = cache.sets[index];
    // Search for a matching tag in the cache set
    for (Slot& slot : cacheSet.slots) {
        if (slot.valid && slot.tag == tag) {
            // Cache hit
            slot.access_ts++;
            slot.load_ts++;
            return 1;
        }
        // even if slot isn't a match, update its load timestamp so that relative order stays the same
        slot.load_ts++;
    } 
    // Determine which cache slot to use for miss 
    int slotToUpdate;
    if (cache.directMapped) {
        slotToUpdate = 0;
    }
    else {
        slotToUpdate = findAvailableSlotIndex(cacheSet);
        // if no available slots can be used to handle miss, then we need to evict based on eviction policy
        if (slotToUpdate == -1) {
            slotToUpdate = findReplacementIndex(cacheSet, lru);
        }
    } 
    // Replace the cache slot with the new data
    Slot& updateSlot = cacheSet.slots[slotToUpdate];
    updateSlot.tag = tag;
    updateSlot.valid = true;
    updateSlot.access_ts = 0;
    updateSlot.load_ts = 0;
    updateSlot.dirty = false;
    // load miss so we have 100 cycles per 4 bytes we had to load from main memory, which is 25 cycle/byte, so 25*bytes loaded
    return 25*data_size;
}

// simulate a cache store, where the return int is the number of cycles the store took 
int cacheStore(Cache& cache, uint32_t index, uint32_t tag, int data_size, bool write_allocate,bool write_through, bool lru, bool* hit) {
    Set& cacheSet = cache.sets[index];
    // Search for a matching tag in the cache set
    for (Slot& slot : cacheSet.slots) {
        if (slot.valid && slot.tag == tag) {
            // Cache hit
            *hit = true;
            slot.access_ts++;
            slot.load_ts++;
            if (write_through) {
                // store to cache is one cycle, store to memory is 25*bytes stored
                return 1+(25*data_size);
            }
            else {
                slot.dirty = true;
                return 1;
            }
        }
        // even if slot isn't a match, update its timestamp
        slot.load_ts++;
    } 
    return handleStoreMiss(cacheSet, tag, data_size, cache.directMapped, write_allocate, write_through, lru);
}

int handleStoreMiss(Set& cacheSet, uint32_t tag, int data_size, bool direct_mapped, bool write_allocate, bool write_through, bool lru) {
    if (!write_allocate) {
        // for no-write-allocate and write-through, we just store to memory
        return 25*data_size;
    }
    int cycles = 0;
    // Determine which cache slot to use for miss 
    int slotToUpdate;
    if (direct_mapped) {
        slotToUpdate = 0;
    }
    else {
        slotToUpdate = findAvailableSlotIndex(cacheSet);
        // if no available slots can be used to handle miss, then we need to evict based on eviction policy
        if (slotToUpdate == -1) {
            slotToUpdate = findReplacementIndex(cacheSet, lru);
        }
    }
    // update the cache slot with the new data
    Slot& updateSlot = cacheSet.slots[slotToUpdate];
    if (write_through || updateSlot.dirty) {
        // we had to load from mem and store to cache if w.t, and write to mem and store to cache if write-back
        cycles = 1+(25*data_size);
    }
    else {
        // if write-back and evicted/updated block isn't dirty, just one cycle
        cycles = 1;
    }
    updateSlot.tag = tag;
    updateSlot.valid = true;
    updateSlot.load_ts = 0;
    updateSlot.access_ts = 0;
    updateSlot.dirty = false;
    return cycles;
}


int findAvailableSlotIndex(Set& cacheSet) {
    for (int i = 0; i < (int) cacheSet.slots.size(); i++) {
        if (cacheSet.slots[i].valid == false) {
            return i;
        }
    }
    return -1;
}

int findReplacementIndex(Set& cacheSet, bool lru) {
    int index = 0;
    if (lru) {
            uint32_t leastUsed = cacheSet.slots[0].access_ts;
            // Iterate through all slots in the set to find the slot with the smallest access_ts.
            for (int i = 1; i < (int) cacheSet.slots.size(); ++i) {
                if (cacheSet.slots[i].access_ts < leastUsed) {
                    leastUsed = cacheSet.slots[i].access_ts;
                    index = i;
                }
            }
        }
    else {
        uint32_t oldest = cacheSet.slots[0].load_ts;
            // Iterate through all slots in the set to find the slot with the smallest access_ts.
        for (int i = 1; i < (int) cacheSet.slots.size(); ++i) {
            if (cacheSet.slots[i].load_ts < oldest) {
                oldest = cacheSet.slots[i].load_ts;
                index = i;
            }
        }
    }
    return index;
}
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
        // ensure positive values
        if (sets <= 0 || blocks <= 0) {
            return false;
        }
        return (checkPowerOfTwo(sets) && checkPowerOfTwo(blocks));
    }
    // if either sets or blocks has an issue reading as an int, not valid 
    catch (std::invalid_argument& e) {
        return false;
    }
}

bool isByteValid(char *byte) {
    try {
        int bytes = std::stoi(byte);
        // ensure positive value greater than or equal to 4
        if (bytes < 4) {
            return false;
        }
        return checkPowerOfTwo(bytes);
    }
    // not valid if input couldn't be read as int 
    catch (std::invalid_argument& e) {
        return false;
    }
}

bool isValidOption(char* input, char* option1, char* option2) {
    int checkOption1 = strcmp(input, option1);
    int checkOption2 = strcmp(input, option2);
    // check if the input matches either option, if so, valid, if not, invalid
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

uint32_t getTag(int bytes, uint32_t address, int sets) {
    // get the offset and index bits
    int offsetBits = log2(bytes);
    // if full associative cache, we want tag to be tag+index bits
    if (sets == 1) {
        uint32_t tag = address >> (offsetBits);
        return tag;
    }
    // if direct mapped or set associative, proceed normally with index and tag
    int indexBits = log2(sets);
    // get the tag by getting the remaining bits of the address not made up of offset or index
    uint32_t tag = address >> (offsetBits + indexBits);
    return tag;
}

uint32_t getIndex(int bytes, uint32_t address, int sets) {
    // index should be zero every time if fully associative cache
    if (sets == 1) {
        return 0;
    }
    // get the offset and index bits
    int offsetBits = log2(bytes);
    int indexBits = log2(sets);
    // make every digit in the index region of address a 1
    uint32_t indexMask = ((1 << indexBits) - 1) << offsetBits;
    // use bitwise AND to get correct digits for index and shift it right to bring index to least significant places 
    uint32_t index = (address & indexMask) >> offsetBits;
    return index;
}

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
            // slots intially have timestamps set to zero
            slot.load_ts = 0;
            slot.access_ts = 0;
            // add slot to current set
            set.slots[j] = slot; 
        }
        // add current set to cache
        cache.sets[i] = set;
    }
    // global timestamp is initally zero
    cache.counter = 0;
    return cache;
}

int cacheLoad(Cache& cache, uint32_t index, uint32_t tag, int data_size, bool write_through, bool lru) {
    Set& cacheSet = cache.sets[index];
    // Search for a matching tag in the cache set
    for (Slot& slot : cacheSet.slots) {
        if (slot.valid && slot.tag == tag) {
            // Cache hit
            cache.counter++;
            slot.access_ts = cache.counter;
            return 1;
        }
    } 
    // Determine which cache slot to use for miss 
    int slotToUpdate;
    slotToUpdate = findAvailableSlotIndex(cacheSet);
    // if no available slots can be used to handle miss, then we need to evict based on eviction policy
    if (slotToUpdate == -1) {
        slotToUpdate = findReplacementIndex(cacheSet, lru);
    }
    int cycles = 0;
    // Replace the cache slot with the new data
    Slot& updateSlot = cacheSet.slots[slotToUpdate];
    // if write_back and dirty, need to write to memory 
    if (updateSlot.dirty && !write_through) {
        cycles += 100*(data_size/4);
    }
    // update appropriate parameters
    updateSlot.tag = tag;
    updateSlot.valid = true;
    cache.counter++;
    updateSlot.access_ts = cache.counter;
    updateSlot.load_ts = cache.counter;
    updateSlot.dirty = false;
    // load miss so we have 100 cycles per 4 bytes we had to load from main memory
    cycles += 100*(data_size/4);
    return cycles;
}

int cacheStore(Cache& cache, uint32_t index, uint32_t tag, int data_size, bool write_allocate, bool write_through, bool lru, bool* hit) {
    Set& cacheSet = cache.sets[index];
    // Search for a matching tag in the cache set
    for (Slot& slot : cacheSet.slots) {
        if (slot.valid && slot.tag == tag) {
            // Cache hit
            *hit = true;
            cache.counter++;
            slot.access_ts = cache.counter;
            if (write_through) {
                // store to memory is 100 cycles
                return 100;
            }
            else {
                // set dirty bit and return one cycle as only cache used
                slot.dirty = true;
                return 1;
            }
        }
    } 
    return handleStoreMiss(cache, cacheSet, tag, data_size, write_allocate, write_through, lru);
}

int handleStoreMiss(Cache& cache, Set& cacheSet, uint32_t tag, int data_size, bool write_allocate, bool write_through, bool lru) {
    if (!write_allocate) {
        // for no-write-allocate and write-through, we just store to memory
        return 100;
    }
    int cycles = 0;
    // Determine which cache slot to use for miss 
    int slotToUpdate;
    slotToUpdate = findAvailableSlotIndex(cacheSet);
    // if no available slots can be used to handle miss, then we need to evict based on eviction policy
    if (slotToUpdate == -1) {
        slotToUpdate = findReplacementIndex(cacheSet, lru);
    }
    // update the cache slot with the new data
    Slot& updateSlot = cacheSet.slots[slotToUpdate];
    // if write_through, we know cache is write allocate write through
    if (write_through) {
        cycles = 100+(100*(data_size/4)); 
    }
    // if evicted block is dirty
    else if (!write_through && updateSlot.dirty) {
        // we had to load from mem and store to cache if w.t, and write to mem and store to cache if write-back
        cycles = 2*(100*(data_size/4));
    }
    else {
        // if write-back and evicted/updated block isn't dirty, just one cycle
        cycles = 100*(data_size/4);
    }
    // update appropriate parameters of slot 
    updateSlot.tag = tag;
    updateSlot.valid = true;
    cache.counter++;
    updateSlot.load_ts = cache.counter;
    updateSlot.access_ts = cache.counter;
    // if write_back, set dirty bit
    if (!write_through) {
        updateSlot.dirty = true;
    }
    else{
        updateSlot.dirty = false;
    }
    return cycles;
}


int findAvailableSlotIndex(Set& cacheSet) {
    // go through set and check if any slot isn't valid, meaning it's open
    for (int i = 0; i < (int) cacheSet.slots.size(); i++) {
        if (cacheSet.slots[i].valid == false) {
            return i;
        }
    }
    // if no slots open, return negative one
    return -1;
}

int findReplacementIndex(Set& cacheSet, bool lru) {
    int index = 0;
    if (lru) {
        uint32_t leastUsed = cacheSet.slots[0].access_ts;
        // Iterate through all slots in the set to find the slot with the smallest access_ts, meaning it was least recently used
        for (int i = 1; i < (int) cacheSet.slots.size(); ++i) {
            if (cacheSet.slots[i].access_ts < leastUsed) {
                leastUsed = cacheSet.slots[i].access_ts;
                index = i;
            }
        }
    }
    else {
        uint32_t oldest = cacheSet.slots[0].load_ts;
            // Iterate through all slots in the set to find the slot with the smallest load_ts, meaning it's the oldest slot in set
        for (int i = 1; i < (int) cacheSet.slots.size(); ++i) {
            if (cacheSet.slots[i].load_ts < oldest) {
                oldest = cacheSet.slots[i].load_ts;
                index = i;
            }
        }
    }
    return index;
}
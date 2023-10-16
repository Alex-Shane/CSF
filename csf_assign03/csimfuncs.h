#include <cstdint>
#include <vector>

#ifndef CSIMFUNCS_H
#define CSIMFUNCS_H

struct Slot {
    uint32_t tag;
    bool valid, dirty;
    uint32_t load_ts, access_ts;
};

struct Set {
    std::vector<Slot> slots;
};

struct Cache {
    std::vector<Set> sets;
    bool directMapped; 
    bool fully_associative;
};

// check that a given number is a power of two
bool checkPowerOfTwo(int num);

// check that given set and block argument are both valid
bool isSetAndBlockValid(char* set, char* block);

// check that bytes per block is valid 
bool isByteValid(char *byte);

// check that a given input argument is one of two valid options
bool isValidOption(char* input, char* option1, char* option2);

// runs through all the above tests, and if any fail, returns false, otherwise true.
bool validParameters(int argc, char** argv);

// get the tag from a current address
uint32_t getTag(int blocks, int bytes, uint32_t address);

// get the index from a current address
uint32_t getIndex(int blocks, int bytes, uint32_t address);

// initialize a cache given cache parameters
Cache initializeCache(int numSets, int numSlots);

// simulate a cache load and return the total cycles taken 
int cacheLoad(Cache& cache, uint32_t index, uint32_t tag, int data_size, bool lru);

// simulate a cache store and return the total cycles taken
int cacheStore(Cache& cache, uint32_t index, uint32_t tag, int data_size, bool write_allocate, bool write_through, bool lru);

// find index of slot to replace after a miss in associative cache
int findReplacementIndex(Set& cacheSet, bool lru);

// find index of available slot in a set
int findAvailableSlotIndex(Set& cacheSet);

// handle the case when a store misses in a cache
int handleStoreMiss(Set& cacheSet, uint32_t tag, int data_size, bool direct_mapped, bool write_allocate, bool write_through, bool lru);

#endif // CSIMFUNCS_H

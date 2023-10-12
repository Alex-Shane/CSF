#include <cstdint>
#include <vector>

#ifndef CSIMFUNCS_H
#define CSIMFUNCS_H

struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts, access_ts;
};

struct Set {
    std::vector<Slot> slots;
};

struct Cache {
    std::vector<Set> sets;
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

// get the number of tag bits for given cache parameters
int findTagBits(int blocks, int bytes);

// initialize a cache given cache parameters
Cache initializeCache(int numSets, int numSlots);

#endif // CSIMFUNCS_H

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
        std::cout << "Valid parameters!" << std::endl;
        std::string input; 
        // read entire input text 
        while (std::getline(std::cin, input)) {
            // create a stream to read 3 sections of each line
            std::istringstream stream(input);
            std::string command, address, ignore;
            // separate stream into 3 distinct sections
            stream >> command >> address >> ignore;
            // check that stream is being sectioned properly
            std::cout << "Command: " << command << ", Address: " << address << std::endl;
        }
        return 0;
    }
    else {
        // error messages dealt with in function, just need to return non zero exit code
        return 1;
    }
}


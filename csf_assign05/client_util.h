#ifndef CLIENT_UTIL_H
#define CLIENT_UTIL_H

#include <string>
class Connection;
struct Message;

// this header file is useful for any declarations for functions
// or classes that are used by both clients (the sender and receiver)

std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

// you can add additional declarations here...

// boolean function that returns true if sending and receiving login message for sender/receiver was succcessful
bool login(const std::string &client, const std::string &username, Connection& conn);

// function to output message to server 
void outputMsg(const std::string &msg);

// helper function to validate msg 
void validateMsg(Message& msg, Connection& conn);

#endif // CLIENT_UTIL_H

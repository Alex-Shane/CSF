#include <iostream>
#include <string>
#include <sstream>
#include "connection.h"
#include "message.h"
#include "client_util.h"

// string trim functions shamelessly stolen from
// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
  return rtrim(ltrim(s));
}

// output message helper for receiver
void outputMsg(const std::string &msg) {
  std::stringstream ss(msg);
  std::string ignore, sender, message;
  // ignore room
  std::getline(ss, ignore, ':');
  // store sender
  std::getline(ss, sender, ':');
  // store message 
  std::getline(ss, message, '\0');
  // print output
  std::cout << sender << ": " << message;
}

// helper function to validate msg 
void validateMsg(Message& msg, Connection& conn) {
  // check the server response and make sure it is valid
  Message server_response = Message();
  conn.receive(server_response);
  // if reponse resulted in tag error or there was invalid message, throw err 
  if (server_response.tag == TAG_ERR || conn.get_last_result() == Connection::INVALID_MSG) {
    std::cerr << server_response.data;
  }
}

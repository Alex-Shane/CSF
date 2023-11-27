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
  std::vector<std::string> msg_info;
  std::stringstream ss(msg);
  std::string item;
  // go through stream of s string and store contents until we reach colon each time
  while (getline(ss, item, ':')) {
    msg_info.push_back(item);
  } 
  std::string user, message;
  // set user and message content appropriately 
  user = msg_info[1];
  message = msg_info[2]; 
  std::cout << user << ": " << message;
}

// helper function to validate msg 
void validateMsg(Message& msg, Connection& conn) {
  // check the server response and make sure it is valid
  Message response = Message();
  conn.receive(response);
  // if reponse resulted in tag error or there was invalid message, throw err 
  if (response.tag == TAG_ERR || conn.get_last_result() == Connection::INVALID_MSG) {
    std::cerr << response.data;
  }
}

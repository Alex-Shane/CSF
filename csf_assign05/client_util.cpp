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

std::vector<std::string> splitMsgData(const std::string &s) {
  std::vector<std::string> splitItems;
  std::stringstream ss(s);
  std::string item;
  // go through stream of s string and store contents until we reach colon each time
  while (getline(ss, item, ':')) {
    splitItems.push_back(item);
  } 
  return splitItems;
}

std::pair<std::string, std::string> deconstructDelivery(std::string msg_data) {
  // split the message data into its components
  std::vector<std::string> items = splitMsgData(msg_data);
  // return the 2nd and 3rd components, representing sender username and message, respectively 
  return { items[1], items[2] };
}

#include <iostream>
#include <string>
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

bool login(const std::string &client, const std::string &username, Connection& conn) {
  Message login_message;
  // set login_message tag based on whether we're logging in receiver or sender 
  if (client == "sender") {
    login_message.tag = TAG_SLOGIN;
  }
  else {
    login_message.tag = TAG_RLOGIN;
  }
  login_message.data = username;

  // attempt to login and catch send failure 
  if (!conn.send(login_message)) {
    std::cerr << "Couldn't send login message" << std::endl;
    conn.close();
    return false;
  }

  // get login response and see if receive failure, return false
  Message login_response = Message();
  if (!conn.receive(login_response)) {
    std::cerr << "Couldn't receive login message" << std::endl;
    conn.close();
    return false;
  }

  // if getting login response failed, print error and return false
  if (login_response.tag == TAG_ERR) {
    std::cerr << login_response.data;
    conn.close();
    return false;
  }

  // if login message was sent and received successfully, we're good
  return true;
}

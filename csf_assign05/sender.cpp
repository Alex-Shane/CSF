#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

// helper function to set tag and data for a command, if invalid command, return false
bool setCommandMsgInfo(const std::string &input, Message& input_msg) {
  // check for quit command
  if (input.substr(0,5) == "/quit") {
    input_msg.tag = TAG_QUIT;
    input_msg.data = "bye";
    return true;
  }
  // check for leave command
  else if (input.substr(0,6) == "/leave") {
    input_msg.tag = TAG_LEAVE;
    input_msg.data = "bye";
    return true;
  }
  // check for join command 
  else if (input.substr(0,5) == "/join") {
    input_msg.tag = TAG_JOIN;
    // data is the remaining part of input
    input_msg.data = input.substr(6);
    return true;
  }
  // if command isn't quit, leave, or join, invalid command and return non zero code
  else {
    std::cerr << "Invalid Command";
    return false;
  }
}

// helper function to send and receive message created from user input, prints error if necessary
void sendAndReceiveMessage(Message& input_msg, Message& server_response, Connection& conn) {
  // send message and check for failure
  if (!conn.send(input_msg)) {
    std::cerr << "Couldn't send the message" << std::endl;
    return;
  }
  // get server response and check for failure to receive 
  if (!conn.receive(server_response)) {
    std::cerr << "Couldn't receive the message" << std::endl;
    return;
  }
  // check for error tag 
  if (server_response.tag == TAG_ERR) {
    std::cerr << server_response.data;
  }
}

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  Connection conn;

  // attempt to connect to server
  conn.connect(server_hostname, server_port);
  // check that server opened successfully
  if (!conn.is_open()) {
    std::cerr << "Couldn't establish connection" << std::endl;
    return 1;
  }

  // if logging in fails, exit program 
  if (!login("sender", username, conn)) {
    return 1;
  }

  // loop through sending messages to server
  while (1) {
    Message input_msg, server_response;;
    std::string input;
    // get the user input from stdin 
    std::getline(std::cin, input);
    // if input wasn't command, it's sendall tag
    if (input[0] != '/') {
      input_msg.tag = TAG_SENDALL;
      input_msg.data = input;
    } 
    // if input is command, set message tag and data based on command 
    else {
      // find what command we have and set message info, if we had invalid command, move onto next message
      if (!setCommandMsgInfo(input, input_msg)) {
        continue;
      }
      // check if our command was quit, if so, follow quit procedure
      else if (input_msg.tag == TAG_QUIT) {
        // send quit message to server
        conn.send(input_msg);
        // receive server response to quit command
        conn.receive(server_response);
        // finish program 
        conn.close();
        return 0;
      }
    }
    // at this point, we have message or valid command, so send message to server and check for failure
    sendAndReceiveMessage(input_msg, server_response, conn);
  }
}

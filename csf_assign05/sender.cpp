#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

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
    return 1;
  }

  // attempt to login and catch if failure to log in
  if (!conn.send(Message(TAG_SLOGIN, username))) {
    std::cerr << "Error: failed to send login message\n";
    conn.close();
    return 1;
  }

  // try to get login response from server
  Message login_response = Message();
  conn.receive(login_response);
  // if error in logging in, close connection and give error message 
  if (login_response.tag == TAG_ERR) {
    std::cerr << login_response.data;
    conn.close();
    return 1;
  }

  // loop through sending messages to server
  while (1) {
      Message msg = Message();
      std::string input;
      // get the user input
      std::getline(std::cin, input);
      // check for quit command
      if (input == "/quit") {
        int quit_result = handleQuit(msg, conn);
        // if quit failed, return error exit code
        if (quit_result == 1) {
          return 1;
        }
        // if quit success, then break out of while loop 
        break;
      }
      // handle leave 
      else if (input == "/leave") {
        msg.tag = TAG_LEAVE;
        msg.data = "bye";
      }
      // handle join
      else if (input.substr(0,5) == "/join") {
        msg.tag = TAG_JOIN;
        // data is the remaining part of input not included in command 
        msg.data = input.substr(6);
      }
      // if command isn't quit, leave, or join, then data is just what user inputted 
      else {
        msg.tag = TAG_SENDALL;
        msg.data = input;
      }
      // send message to server communicating the input and if send failed, throw error
      if (!conn.send(msg)) {
        std::cerr << "Error: failed to send message\n";
        return 1;
      }
      // ensure msg and server response is valid, if not, print error
      validateMsg(msg, conn);
    }
    
    // close connection 
    conn.close();
    return 0;
}

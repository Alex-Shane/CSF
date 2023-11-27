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
    std::cerr << "Couldn't establish connection\n";
    return 1;
  }

  // attempt to login and catch if failure to log in
  Message login_message = Message(TAG_SLOGIN, username);
  if (!conn.send(login_message)) {
    std::cerr << "Couldn't send login message\n";
    conn.close();
    return 1;
  }

  // try to get login response from server
  Message login_response = Message();
  conn.receive(login_response);
  // check that tag isn't error
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
      if (input.substr(0,5) == "/quit") {
        msg.tag = TAG_QUIT;
        msg.data = "bye";
        // send quit message to server
        conn.send(msg);
        // finish program 
        conn.close();
        return 0;
      }
      // handle leave 
      else if (input.substr(0,6) == "/leave") {
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
      // send message to server
      conn.send(msg);
      // ensure msg is valid and server response wasn't error
      validateMsg(msg, conn);
    }
    
    // close connection 
    conn.close();
    return 0;
}

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

  // attempt to login
  bool login = conn.send(Message(TAG_SLOGIN, username));
  // catch failure to log in
  if (!login) {
    std::cerr << "Error: failed to send login message to server\n";
    return 1;
  }

  // try to login 
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
      std::cout << input << '\n';
      // check for quit command
      if (input == "/quit") {
        // set tag and data
        msg.tag = TAG_QUIT;
        msg.data = "bye";
        // try to quit out of sender, if not, throw error 
        bool quit = conn.send(msg);
        if (!quit) {
          std::cerr << "Error: failed to send quit message to server\n";
          return 1;
        }
        // get the server's response
        Message response;
        conn.receive(response);
        // check for error in response or invalid message sent 
        if (response.tag == TAG_ERR || conn.get_last_result() == Connection::INVALID_MSG) {
          std::cerr << response.data;
          return 1;
        }
        // break out of program no matter what since quit command 
        break;
      }
      // handle leave 
      if (input == "/leave") {
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
        std::cout << "successfully reached sendall" << '\n';
        msg.tag = TAG_SENDALL;
        msg.data = input;
      }
      // send message to server communicating the input
      bool success = conn.send(msg);
      // if send failed, throw error
      if (!success) {
        std::cerr << "Error: failed to send main loop message\n";
        return 1;
      }
      // check the server response and make sure it is valid
      Message response = Message();
      conn.receive(response);
      // if reponse resulted in tag error or there was invalid message, throw err 
      if (response.tag == TAG_ERR || conn.get_last_result() == Connection::INVALID_MSG) {
        std::cerr << response.data;
      }
    }
    
    // close connection 
    conn.close();
    return 0;
}

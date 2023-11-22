#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <tuple>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;
  // attempt to connect to server
  conn.connect(server_hostname, server_port);
  // check that server opened successfully
  if (!conn.is_open()) {
    return 1;
  }

  // attempt to login
  bool login = conn.send(Message(TAG_RLOGIN, username));
  // catch failure to log in
  if (!login) {
    std::cerr << "Error: failed to send login message to server\n";
    return 1;
  }

  // get login response
  Message login_response = Message();
  conn.receive(login_response);
  // if getting login response failed, catch it
  if (login_response.tag == TAG_ERR) {
    std::cerr << login_response.data;
    return 1;
  }

  // try to join room
  bool success = conn.send(Message(TAG_JOIN, room_name));
  if (!success) {
    std::cerr << "Error: failed to send join message to server\n";
    return 1;
  }

  // get join response 
  Message join_response = Message();
  conn.receive(join_response);
  if (join_response.tag == TAG_ERR) {
    std::cerr << join_response.data;
    return 1;
  }

  // wait for messages from server
  while (1) {
    Message msg = Message();
    bool success = conn.receive(msg);
    // make sure message received successfully
    if (!success) {
      break;
    }
    // output message only if tag is delivery
    if (msg.tag == TAG_DELIVERY) {
      std::string sender, message;
      // split the message data into its components
      std::vector<std::string> msg_info = splitMsgData(msg.data);
      sender = msg_info[1];
      message = msg_info[2]; 
      std::cout << sender << ": " << message;
    }
  }

  // close connection when loop terminated
  conn.close();
  return 0;
}

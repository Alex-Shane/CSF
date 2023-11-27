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
    std::cerr << "Couldn't establish connection\n";
    return 1;
  }

  // attempt to login and catch if failure to log in
  Message login_message = Message(TAG_RLOGIN, username);
  if (!conn.send(login_message)) {
    std::cerr << "Couldn't send login message\n";
    conn.close();
    return 1;
  }

  // get login response
  Message login_response = Message();
  conn.receive(login_response);
  // if getting login response failed, print error and exit
  if (login_response.tag == TAG_ERR) {
    std::cerr << login_response.data;
    conn.close();
    return 1;
  }

  // try to join room and catch error and exit if needed
  Message join_message = Message(TAG_JOIN, room_name);
  if (!conn.send(join_message)) {
    std::cerr << "Couldn't send join message\n";
    conn.close();
    return 1;
  }

  // get join response 
  Message join_response = Message();
  conn.receive(join_response);
  // if joined failed, exit
  if (join_response.tag == TAG_ERR) {
    std::cerr << join_response.data;
    conn.close();
    return 1;
  }

  // wait for messages from server
  while (1) {
    Message msg = Message();
    // take in message from server 
    conn.receive(msg);
    // output message only if tag is delivery
    if (msg.tag == TAG_DELIVERY) {
      outputMsg(msg.data);
    }
    // if not delievery, error and must output payload 
    else {
      std::cerr << msg.data;
    }
  }

  // close connection when loop terminated
  conn.close();
  return 0;
}

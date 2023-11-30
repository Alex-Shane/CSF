#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

void outputMessage(const std::string &server_msg_data) {
  std::string ignore, sender, message;
  // create stream to go through data of expected form room:sender:message
  std::stringstream ss(server_msg_data);
  // ignore room and check if getline failed 
  if (!(std::getline(ss, ignore, ':'))) {
    std::cerr << "Error reading delievery" << std::endl;
    // stop function
    return;
  }
  // store sender by going through stream until next colon, and check if getline failed
  if (!(std::getline(ss, sender, ':'))) {
    std::cerr << "Error reading delievery" << std::endl;
    // stop function
    return;
  }
  // store message by getting the rest of the message in the stream, and check if getline failed
  if (!(std::getline(ss, message, '\0'))) {
    std::cerr << "Error reading delievery" << std::endl;
    // stop function
    return;
  }
  // print output
  std::cout << sender << ": " << message;
}

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
    std::cerr << "Couldn't establish connection" << std::endl;
    return 1;
  }

  // ensure login was successful, if not exit 
  if (!login("receiver", username, conn)) {
    return 1;
  }

  // try to join room and catch error and exit if needed
  Message join_message = Message(TAG_JOIN, room_name);
  if (!conn.send(join_message)) {
    std::cerr << "Couldn't send join message" << std::endl;
    conn.close();
    return 1;
  }

  // get join response 
  Message join_response;
  if (!conn.receive(join_response)) {
    std::cerr << "Couldn't receive join message" << std::endl;
    conn.close();
    return 1;
  }
  // if joined has error tag, print err and exit
  if (join_response.tag == TAG_ERR) {
    std::cerr << join_response.data;
    conn.close();
    return 1;
  }

  // wait for messages from server
  while (1) {
    Message server_msg;
    // try to receive message from server, print error
    if (!conn.receive(server_msg)) {
      std::cerr << "Couldn't receive message" << std::endl;
    }
    // output message only if tag is delivery
    else if (server_msg.tag == TAG_DELIVERY) {
      outputMessage(server_msg.data);
    }
    // if not delievery, check for error tag
    else if (server_msg.tag == TAG_ERR){
      std::cerr << server_msg.data;
    }
  }
}

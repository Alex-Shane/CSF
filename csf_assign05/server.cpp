#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// struct to store data needed to communicate with client 
struct Client_Helper {
  // struct variables
  Connection* conn;
  Server* server;
  User* user;

  // constructor to init everything
  Client_Helper(Connection* c, Server* s, User* u): conn(c), server(s), user(u) { }

  // destructor frees connection and user memory, still need server for later in program
  ~Client_Helper() {
    delete conn;
    delete user;
  }
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

// helper function to handle server communication with a sender
void chat_with_sender(Client_Helper* ch) {
  // room is initially set to null before client requests a room
  Room* room = nullptr;
  while (true) {
    // receive message from client and check for failure
    Message message_from_client;
    if (!ch->conn->receive(message_from_client)) {
      // handle invalid msg
      if (ch->conn->get_last_result() == Connection::INVALID_MSG) {
        Message server_error_response = Message(TAG_ERR, "Invalid message, please try again with different message");
        ch->conn->send(server_error_response);
      }
      // exit out of sender 
      return;
    }

    // check for quit tag
    if (message_from_client.tag == TAG_QUIT) {
      Message server_response = Message(TAG_OK, "bye");
      ch->conn->send(server_response);
      // if quit, and sender is in a room, must remove them from room
      if (room != nullptr) {
        room->remove_member(ch->user);
      }
      // exit out of thread
      return;
    }

    // check for join tag
    else if (message_from_client.tag == TAG_JOIN) {
      // if sender in room currently, remove them first before joining new room
      if (room != nullptr) {
        room->remove_member(ch->user);
      }
      std::string room_name = message_from_client.data;
      // delete last character ('\n') from room_name
      room_name.erase(room_name.size()-1);
      std::string join_message = "Successfuly joined " + room_name;
      Message server_response = Message(TAG_OK, join_message);
      ch->conn->send(server_response);
      // register sender to existing room, or make one
      room = ch->server->find_or_create_room(room_name);
      // add user to room 
      room->add_member(ch->user);
    }

    // check for leave tag
    else if (message_from_client.tag == TAG_LEAVE) {
      // ensure there is room to leave from, if not then error
      if (room == nullptr) {
        Message server_error_response = Message(TAG_ERR, "You cannot leave a room you haven't joined");
        ch->conn->send(server_error_response);
      } 
      // if room exists, de register sender from room
      else {
        // remove sender from room and set room back to null
        room->remove_member(ch->user);
        room = nullptr;
        // send server_response
        Message server_response = Message(TAG_OK, "Successfuly left the room");
        ch->conn->send(server_response);
      }
    }

    // check for send all tag
    else if (message_from_client.tag == TAG_SENDALL) {
      // ensure there is a room to send message to, if not then error
      if (room == nullptr) {
        Message server_error_response = Message(TAG_ERR, "You cannot send message to a room you haven't joined");
        ch->conn->send(server_error_response);
      }
      // if room exists, broadcast message to entire room
      else {
        std::string sender_msg_text = message_from_client.data;
        // delete last character ('\n') from msg text 
        sender_msg_text.erase(sender_msg_text.size()-1);
        // send message to everyone in the room, then send server response
        room->broadcast_message(ch->user->username, sender_msg_text);
        Message server_response = Message(TAG_OK, "Message sent");
        ch->conn->send(server_response);
      }
    }

    // if tag isn't join, leave, quit, sendall, then invalid tag
    else {
      Message server_error_response = Message(TAG_ERR, "Invalid tag provided, try again");
      ch->conn->send(server_error_response);
    }
  }
}

void chat_with_receiver(Client_Helper* ch) {
  // initially set room to null
  Room* room = nullptr;
  Message received_msg;
  // try to receive first msg from receiver, check for failure
  if (!(ch->conn->receive(received_msg))) {
    // handle invalid msg
    if (ch->conn->get_last_result() == Connection::INVALID_MSG) {
      Message server_error_response = Message(TAG_ERR, "Invalid message, please try again with different message");
      // send error message 
      ch->conn->send(server_error_response);
    }
    // need to return for both invalid msg and error parsing
    return;
  }

  // only valid message we can receive at this point is a join tag, if not then error
  if (received_msg.tag != TAG_JOIN) {
    Message server_response = Message(TAG_ERR, "Must join room before sending message");
    ch->conn->send(server_response);
    return;
  }

  // if join tag, enter receiver into room 
  else {
    std::string room_name = received_msg.data;
    // delete last character ('\n') from room_name
    room_name.erase(room_name.size()-1);
    // set room to current room that has room_name, if no rooms match criteria, new room is made with room_name
    room = ch->server->find_or_create_room(room_name);
    // add user to room
    room->add_member(ch->user);
    // send server response
    std::string join_msg = "Successfully joined " + room_name;
    Message server_response = Message(TAG_OK, join_msg);
    ch->conn->send(server_response);
  }

  // now we listen for messages sent into the room to be broadcast 
  while (true) {
    // get message, if possible
    Message* received_msg = ch->user->mqueue.dequeue();
    // if we received a message, try to send it 
    if (received_msg != nullptr) {
      // if send fails, clean up memory, remove member from room, and exit thread
      if (!(ch->conn->send(*received_msg))) {
        delete received_msg;
        // if send fails, we lost connection so remove user from room and exit
        room->remove_member(ch->user);
        return;
      }
      // if send was successful, still need to delete message
      delete received_msg;
    }

    // if we didn't receive message, the queue was too busy and timed out and we just continue
    else {
      continue;
    }
  }
}

void *worker(void *arg) {
  pthread_detach(pthread_self());
  // init Client_Helper through static cast
  Client_Helper* ch = static_cast<Client_Helper*>(arg); 

  // read login message and check if it was received properly, if not, send server response and exit with nullptr
  Message login_message;
  if (!ch->conn->receive(login_message)) {
    Message server_error_response = Message(TAG_ERR, "Couldn't receive login message");
    ch->conn->send(server_error_response);
    return nullptr;
  }

  // if valid login, handle it
  if (login_message.tag == TAG_RLOGIN || login_message.tag == TAG_SLOGIN) {
    Message server_response = Message(TAG_OK, "Successfully logged in!");
    ch->conn->send(server_response);
    std::string username = login_message.data;
    // delete last char ('\n') from username
    username.erase(username.size()-1);
    // create user for thread
    User* user = new User(username);
    // set user in client helper
    ch->user = user;
    // if receiver login, communicate with receiver
    if (login_message.tag == TAG_RLOGIN) {
      chat_with_receiver(ch);
    }
    // if sender login, communicate with sender
    else {
      chat_with_sender(ch);
    }
    return nullptr;
  }

  // if we didn't get rlogin or slogin tag, invalid attempt at message and exit with nullptr
  else {
    Message server_error_response = Message(TAG_ERR, "Invalid message: try logging in before sending a message");
    ch->conn->send(server_error_response);
    return nullptr;
  }

  // don't think it will ever get here but need it for compilation
  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // init mutex
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // destroy mutex
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // turn m_port into a string
  std::string m_port_str = std::to_string(m_port);
  // set m_ssock based on open_listenfd result
  m_ssock = open_listenfd(m_port_str.c_str());
  // ensure open_listenfd was successful, if not, return false
  if (m_ssock < 0) {
    return false;
  }
  return true;
}

void Server::handle_client_requests() {
  // infinite loop to create pthreads for clients until server is shut down 
  while (1) {
    // accept a new file descriptor
    int fd = Accept(m_ssock, nullptr, nullptr);
    // if fd is negative, we have error, and server shuts down
    if (fd < 0) {
      std::cerr << "File descriptor didn't open properly." << std::endl;
      return;
    } 

    // make Client and initialize its data (server = this, user = nullptr)
    Connection* conn = new Connection(fd);
    Client_Helper* ch = new Client_Helper(conn, this, nullptr);

    // Create a new thread for the client and call the worker function
    // if pthread_create fails, we shut down server and deal with memory from client_helper 
    pthread_t client_thread;
    if (pthread_create(&client_thread, nullptr, worker, static_cast<void*>(ch)) != 0) {
        std::cerr << "Failed to create thread." << std::endl;
        delete ch;
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // make critical section
  Guard g(m_lock);
  // check to see if room_name is a currently mapped to a room
  auto roomExists = m_rooms.find(room_name);

  // if it is mapped to a room (ie the iterator is defined and not the end of the map), return that room
  if (roomExists != m_rooms.end()) {
    return roomExists->second;
  }
  // if it's not mapped, make a room for it and return the new room
  else {
    Room* new_room = new Room(room_name);
    this->m_rooms[room_name] = new_room;
    return new_room;
  }
}

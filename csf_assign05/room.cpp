#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"
#include <iostream>

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // init mutex
  pthread_mutex_init(&lock, nullptr);
}

Room::~Room() {
  // destroy mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // make critical section with guard
  Guard g(lock);
  // add user to room
  members.insert(user);
}

void Room::remove_member(User *user) {
  // make critical section with guard
  Guard g(lock);
  // remove user from room
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // make broadcast message string
  std::string msg_string = this->room_name + ":" + sender_username + ":" + message_text;
  // set broadcoast message info
  Message* broadcast_msg = new Message(TAG_DELIVERY, msg_string);
  // make critical section to add message to user queues
  Guard g(lock);
  // for each user in room, add message to their queue
  for (User* u : members) {
    u->mqueue.enqueue(broadcast_msg);
  }
}

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
  // make critical section to add message to user queues
  Guard g(lock);
  // make broadcast message string
  std::string msg_string = this->room_name + ":" + sender_username + ":" + message_text;
  // for each user in room, add message to their queue
  for (User* u : members) {
    // set broadcast message pointer and then add it to the user's queue
    Message* broadcast_msg = new Message(TAG_DELIVERY, msg_string);
    u->mqueue.enqueue(broadcast_msg);
  }
}

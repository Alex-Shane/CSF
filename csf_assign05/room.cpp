#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // TODO: initialize the mutex
  // init mutex
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  // TODO: destroy the mutex
  // destroy mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // TODO: add User to the room
  // lock mutex and make critical section with guard
  Guard g(lock);
  // add user to room
  members.insert(user);
}

void Room::remove_member(User *user) {
  // TODO: remove User from the room
  // lock mutex and make critical section with guard
  Guard g(lock);
  // remove user from room
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // TODO: send a message to every (receiver) User in the room
  // make broadcast message
  Message broadcast_msg;
  broadcast_msg.tag = TAG_DELIVERY;
  broadcast_msg.data = this->room_name + ":" + sender_username + ":" + message_text;
  // make guard and critical section
  Guard g(lock);
  for (User* u : members) {
    u->mqueue.enqueue(&broadcast_msg);
  }
}

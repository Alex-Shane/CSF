#include <sstream>
#include <cctype>
#include <cassert>
#include <string>
#include <iostream>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  rio_readinitb(&m_fdbuf, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  m_fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
  // initialize rio_t obj if successful connection 
  if (m_fd >= 0) {
    rio_readinitb(&m_fdbuf, m_fd);
  }
}

Connection::~Connection() {
  // close call handles checking if connection open or not
  close();
}

bool Connection::is_open() const {
  return m_fd >= 0;
}

void Connection::close() {
  // close the connection if it is open
  if (is_open()) {
    Close(m_fd);
    // set fd to -1 so is_open actually works
    m_fd = -1;
  }
}

bool Connection::send(const Message &msg) {
  // turn msg into string
  std::string msg_str = msg.tag + ":" + msg.data + "\n";
  size_t msg_size = msg_str.size();
  // check if msg size is valid 
  if (msg_size > Message::MAX_LEN) {
    m_last_result = INVALID_MSG;
    return false; 
  }
  // write message 
  ssize_t bytes_writen = rio_writen(m_fd, msg_str.c_str(), msg_str.size());
  // if there was error with rio_written, set last result to error state
  if (bytes_writen != static_cast<ssize_t>(msg_str.size())) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }
  // if no error, we have success
  m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  // allocate char array that can hold largest message 
  char buf[Message::MAX_LEN];
  // read message 
  ssize_t bytes_read = rio_readlineb(&m_fdbuf, buf, Message::MAX_LEN);
  // check for error in reading
  if (bytes_read < 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }
  // make message string and assign it to the buf array 
  std::string message;
  message.assign(buf);
  // find colon to determine when tag ends and data/payload starts 
  int tag_end = message.find(':');
  // use colon index to get tag and data
  msg.tag = message.substr(0,tag_end);
  msg.data = message.substr(tag_end + 1);
  // return that we successfully received message 
  m_last_result = SUCCESS;
  return true;
}

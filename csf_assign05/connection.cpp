#include <sstream>
#include <cctype>
#include <cassert>
#include <string>
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
  m_fd = open_clientfd(hostname.cstring(), std::to_string(port).cstring());
  // check if connection failed 
  if (m_fd < 0) {
    std::cerr << "Error: Connection failed";
  }
  // initialize rio_t obj 
  rio_readinitb(&m_fdbuf, fd);
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
  }
}

bool Connection::send(const Message &msg) {
  // does this account for both "\n" and "\r\n" delimiters?
  std::string msg_str = msg.tag + ":" + msg.data + "\n";
  size_t msg_size = msg_str.size();
  // check if msg size is valid 
  if (msg_size > MAX_LEN) {
    m_last_result = INVALID_MSG;
    return false; 
  }
  ssize_t result = rio_writen(m_fd, msg_str.c_str(), msg_str.size());
  // if there was error with rio_written, set last result to error state
  if (result < 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }
  // if no error, we have success
  m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  char msg_buf[256];
  ssize_t read_result = rio_readlineb(&m_fdbuf, msg_buf, 256);
  // if error in reading, then message was too long and thus invalid
  if (receive_result < 0) {
    m_last_result = INVALID_MSG;
    return false;
  }
  std::string message(msg_buf);
  int tag_end = message.find(':');
  // use colon index to get tag and data
  msg.tag = message.substr(0,tag_end);
  msg.data = message.substr(tag_end + 1);
  // return that we successfully received message 
  m_last_result = SUCCESS;
  return true;
}

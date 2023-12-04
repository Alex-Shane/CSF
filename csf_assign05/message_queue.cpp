#include <cassert>
#include <ctime>
#include <semaphore.h>
#include "guard.h"
#include "message_queue.h"
#include "message.h"

MessageQueue::MessageQueue() {
  // init mutex
  pthread_mutex_init(&m_lock, nullptr);
  // init semaphore
  sem_init(&m_avail, 0, 0);
}

MessageQueue::~MessageQueue() {
  // create block scoped critical section
  {
    Guard g(m_lock);
    // for each message left in queue, free the memory 
    for (Message* m : m_messages) {
      delete m;
    }
  }
  // destroy mutex
  pthread_mutex_destroy(&m_lock);
  // destroy semaphore
  sem_destroy(&m_avail);
}

void MessageQueue::enqueue(Message *msg) {
  // make block scoped critical section, lock mutex
  {
    Guard g(m_lock);
    // put message on queue
    m_messages.push_back(msg);
  }
  // notify threads with sem_post that message is available
  sem_post(&m_avail);
}

Message *MessageQueue::dequeue() {
  struct timespec ts;

  // get the current time using clock_gettime:
  // we don't check the return value because the only reason
  // this call would fail is if we specify a clock that doesn't
  // exist
  clock_gettime(CLOCK_REALTIME, &ts);

  // compute a time one second in the future
  ts.tv_sec += 1;

  // call sem_timed wait and return nullptr if it times out 
  int result = sem_timedwait(&m_avail, &ts);
  if (result == -1) {
    return nullptr;
  }

  Message *msg = nullptr;
  // create block scoped critical section to remove the message from queue
  {
    Guard g(m_lock);
    // store msg to remove
    msg = m_messages.front();
    // remove the msg
    m_messages.pop_front();
  }
  // return the message
  return msg;
}

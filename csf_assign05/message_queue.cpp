#include <cassert>
#include <ctime>
#include <semaphore.h>
#include "guard.h"
#include "message_queue.h"

MessageQueue::MessageQueue() {
  // TODO: initialize the mutex and the semaphore
  // init mutex
  pthread_mutex_init(&m_lock, NULL);
  // init semaphore
  sem_init(&m_avail, 0, 0);
}

MessageQueue::~MessageQueue() {
  // TODO: destroy the mutex and the semaphore
  // destroy mutex
  pthread_mutex_destroy(&m_lock);
  // destroy semaphore
  sem_destroy(&m_avail);
  for (Message* m : m_messages) {
    delete m;
  }
}

void MessageQueue::enqueue(Message *msg) {
  // TODO: put the specified message on the queue
  // make critical section and lock mutex
  Guard g(m_lock);
  // put message on queue
  m_messages.push_back(msg);
  // notify threads with sem_post
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

  // TODO: call sem_timedwait to wait up to 1 second for a message
  //       to be available, return nullptr if no message is available
  // make critical section
  // lock mutex
  Guard g(m_lock);
  // call sem_timed wait and return nullptr if it times out 
  int result = sem_timedwait(&m_avail, &ts);
  if (result == -1) {
    return nullptr;
  }
  // TODO: remove the next message from the queue, return it
  // store msg to remove
  Message *msg = m_messages.front();
  // remove the msg
  m_messages.pop_front();
  // return the message
  return msg;
}

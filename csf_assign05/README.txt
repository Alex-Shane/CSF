
MS1:
    Alex work: receiver and connection and refactoring
    Dylan work: sender  

MS2: Alex work: worker, chat with receiver/sender, readme report 
     Dylan work: server member functions, room, and message queue

Critical sections are the sections of our code that deal with shared data between threads and need to be completed without interruption from other threads. 
In order to acheive synchronization between threads and have the server run smoothly, we had to create multiple of these critical sections and employ different 
synchronization primitives to ensure the server could handle edge cases such as data races and deadlocks. Our critical sections and how they followed synchronization protocol and prevented hazards are below:

section #1: in server.cpp, we had critical section when finding or creating room, since the room data is shared amongst all threads. For this section, we only employed
a mutex to ensure there would be no data race if two threads attempted to find a room simultaneously and we ensure no deadlocks by using a Guard object. 

section #2: in room.cpp, we had critical section when adding member to room since we were dealing with shared member/user data. For this section,
we just used a mutex since the Guard we used it with prevents any potential deadlocks and the mutex itself ensures that only one thread at a time can access/be added to the room,
thus preventing data races. 

section #3: in room.cpp, we had critical section when removing member from room since dealing with shared member data. We employed a mutex/guard combo so that only one thread
could remove member from room at a time and deadlocks were prevented due to protection of using a guard. 

section #4: in room.cpp, we had critical section when when broadcasting a message since this section dealt with shared member data. We again used a mutex/guard combo
so that we could send a unique pointer to each client in room, preventing potential memory issues from a data race with maybe multiple clients having a pointer to the same message. 

section #5: in message_queue.cpp, we had critical section in the queue destructor when we free each message because of shared access to the message queue. For this section, we used
a mutex/guard combo, but also made the section block scoped as we needed the guard to unlock our mutex once it finished freeing all the messages so that the mutex would be unlocked 
when we called to destroy the mutex. Similar to the other sections, the mutex/guard combo prevented dangerous memory situations such as deleting a message while another thread is accessing
that same message and added the safety of knowing the mutex would always lock/unlock as desired to allow for future thread access. 

section #6: in message_queue.cpp, we had critical section in the enqueue method since there was access to the shared queue of messages. By using a guard/mutex combo here,
we ensured that only one thread pushed a message onto the queue at a time, preventing duplicate messages from being added to queue which would corrupt the queue. We also block scoped this 
section, leaving sem post outside the critical section in order to minimize the time that the mutex is locked, and thus allowing the semaphore to notify other threads that another message can
be processed in a quicker manner. 

section #7: in message_queue.cpp, we had critical section in the dequeue method since there was access to the shared queue of messages. Again, we used block scoped guard/mutex combo so that only
the portion of code dealing with the shared queue message was protected against. We didn't guard before sem_timedwait as doing so would reduce concurrency and potential add delays to the server
as compared to locking the mutex after we receive the result of timed wait. Moreover, by guarding the mutex, we ensure only one thread can remove a message from the queue at a time, preventing data 
races that could result in the same message being removed by multiple threads which is very bad. 

In general, all our critical sections were protected with a mutex/guard combo to prevent simultaneous access of shared data and any blocking between threads. We used
block scoped critical sections when we wanted to allow for more concurrency in other parts of a function that didn't deal with shared data. Lastly, although we used semaphores to help
in our synchronization efforts, they were not included in our critical sections as they didn't directly access shared data (sem post increments # messages available to threads, sem wait decrements # messages available), 
and keeping them outside our critical sections allowed for the mutex to be locked for less time, which in turn makes the program slightly more efficient. We are pretty confident in our synchronization 
approach as we passed both the automated test script for concurrency and all the gradescope tests for concurrency. 

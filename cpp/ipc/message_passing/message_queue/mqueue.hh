#ifndef MQUEUE_HH_
#define MQUEUE_HH_

#include <sys/ipc.h>  // key_y, ftok, IPC_CREAT
#include <sys/msg.h>  // msgget, msgsnd

#include <cstring>
#include <iostream>

namespace MessageQueue {

class Task {
 public:
  virtual void run() const = 0;
  virtual ~Task() = default;
};

class Queue {
 private:
  static constexpr int project_id{65};
  struct message_buffer {
    long type;
    char text[20];
  };

 protected:
  int queue_id;
  mutable struct message_buffer msg;
  static constexpr std::streamsize buffer{20};

 public:
  Queue() : queue_id{create_queue()}, msg{} {}
  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;
  Queue(Queue&&) = delete;
  Queue& operator=(Queue&&) = delete;

 protected:
  bool terminate() const { return strncmp(msg.text, "0", 1) == 0; }

 private:
  static int create_queue() {
    // ftok to generate a random key
    key_t key = ftok("progfile", project_id);
    // msgget creates a message queue, and returns identifier
    return msgget(key, 0666 | IPC_CREAT);
  }
};

class Write : public Queue, public virtual Task {
 public:
  Write() : Queue() { msg.type = 1; }
  void run() const {
    do {
      std::cout << "Insert data: ";
      std::cin.getline(msg.text, buffer);
      // send message to message queue
      msgsnd(queue_id, &msg, buffer, 0);
      // Display the message
      std::cout << "Data sent to queue '" << msg.text << "'" << std::endl;
    } while (!terminate());
  }
};

class Read : public Queue, public virtual Task {
 public:
  Read() : Queue() { msg.type = 0; }
  ~Read() { msgctl(queue_id, IPC_RMID, nullptr); }
  void run() const {
    do {
      msgrcv(queue_id, &msg, buffer, 1, 0);
      // Display the message
      std::cout << "Data received from queue: " << msg.text << std::endl;
    } while (!terminate());
  }
};

}  // namespace MessageQueue

#endif
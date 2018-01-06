#ifndef THREADS_H_
#define THREADS_H_

#include "blocking_queue.h"

#include <thread>
#include <vector>
    
class ThreadPool {
  using Task = std::function<void()>;

 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  void Start();
  void Join();
  ThreadPool& Exit() { tasks_.exit(); running_ = false; return *this; }

  void AddTask(Task&& task);
  void AddTask(const Task& task) { return AddTask(Task(task)); }
  size_t TaskSize() const noexcept { return tasks_.size(); }

 private:
  BlockingQueue<Task> tasks_;
  std::vector<std::thread> threads_;
  std::atomic<bool> running_;
};

#endif  // THREADS_H_


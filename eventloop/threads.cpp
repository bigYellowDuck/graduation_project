#include "threads.h"
#include "logging.h"

#include <assert.h>

ThreadPool::ThreadPool(size_t threads)
   : tasks_(),
     threads_(threads),
     running_(false) {
  assert(threads >= 0);
  info("ThreadPool has %lu threads", threads);
}

ThreadPool::~ThreadPool() {
  assert(tasks_.exited() && !running_);
  if (tasks_.size()) {
    info("%lu tasks not processed when thread pool exited\n", 
         tasks_.size());
  }
}

void ThreadPool::Start() {
  if (running_ || threads_.size() <= 0) {
    return;
  }

  for (auto& th : threads_) {
    std::thread t(
      [this]() {
        while (!tasks_.exited()) {
          Task task;
          if (tasks_.pop(&task))
            task();
        }
      });   
    th.swap(t);
  }

  running_ = true;

  info("ThreadPool is running %lu threads", threads_.size());
}

void ThreadPool::Join() {
  for (auto& t : threads_) {
    t.join();
  }
}

void ThreadPool::AddTask(Task&& task) {
  tasks_.push(std::move(task));
}


#include "thread_pool.hpp"

#include <chrono>
#include <cstddef>
#include <mutex>
#include <thread>

namespace vgd {
thread_pool::thread_pool(size_t thread_number)
    : thread_number_(thread_number), threads_(thread_number_) {
  for (size_t i = 0; i < thread_number_; ++i) {
    std::thread thread_t(&thread_pool::per_thread_func, this);
    threads_[i].swap(thread_t);
  }
}

thread_pool::~thread_pool() {
  stop();
  for (size_t i = 0; i < thread_number_; ++i) {
    threads_[i].join();
  }
}

void thread_pool::stop() {
  std::lock_guard lg(task_queue_M_);
  is_end = true;
  task_queue_wait_.notify_all();
}

void thread_pool::per_thread_func() {
  while (!is_end) {
    task_type task;
    {
      std::unique_lock<std::mutex> ul(task_queue_M_);
      while (task_queue.empty()) {
        if (is_end) {
          return;
        }
        task_queue_wait_.wait(ul);
      }
      task = task_queue.front();
      task_queue.pop_front();
      if (!task_queue.empty()) {
        task_queue_wait_.notify_one();
      }
    }
    task();
    --task_number;
  }
}

void thread_pool::push_task(task_type&& task) {
  std::lock_guard<std::mutex> lg(task_queue_M_);
  task_queue.push_back(task);
  ++task_number;
  task_queue_wait_.notify_one();
}

void thread_pool::wait_task_done() {
  while (task_number != 0) {
  }
}

}  // namespace vgd
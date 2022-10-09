
#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
namespace vgd {

class thread_pool {
 private:
  using task_type = std::function<void()>;

  const size_t thread_number_;

  std::list<task_type> task_queue;
  std::mutex task_queue_M_;
  std::condition_variable task_queue_wait_;

  bool is_end{false};

  void per_thread_func();

  std::atomic<unsigned long int> task_number{0};

  std::vector<std::thread> threads_;

 public:
  thread_pool(size_t thread_number);
  void stop();
  void push_task(task_type&& task);
  void wait_task();
  ~thread_pool();
};

}  // namespace vgd

#endif  // THREAD_POOL_HPP
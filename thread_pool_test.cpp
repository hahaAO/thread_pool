#include "thread_pool/thread_pool.hpp"

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <iostream>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <thread>
// TODO: replace usage of global namespace (std)
using namespace std;

#include <vgd_tools/timer.hpp>
// TODO: replace usage of global namespace (std::chrono)
using namespace std::chrono;
std::shared_mutex rwm;
std::mutex m;

inline bool one_in_four() {
  // There is a one-in-four chance of an increase
  static std::minstd_rand random_seed(time(NULL));
  static std::uniform_int_distribution<size_t> dis1(1, 4);
  if (dis1(random_seed) == 4) {
    return true;
  }
  // for (size_t i = 0; i < 100; ++i) {
  //   dis1(random_seed);
  // }
  return false;
}

void func_shared_mutex() {
  std::shared_lock<std::shared_mutex> gl(rwm);
  one_in_four();
}
void func_mutex() {
  std::lock_guard<std::mutex> gl(m);
  one_in_four();
}
void func_no_mutex() { one_in_four(); }
int testNumber = 100000;
int thread_num = 4;
int main() {
  vgd::thread_pool thread_pool(thread_num);
  boost::asio::thread_pool thread_pool2(thread_num);

  {
    vgd::Timer t("boost rwm");
    for (size_t i = 0; i < testNumber; ++i) {
      boost::asio::post(thread_pool2, func_shared_mutex);
    }
  }

  {
    vgd::Timer t("rwm");
    for (size_t i = 0; i < testNumber; ++i) {
      thread_pool.push_task(func_shared_mutex);
    }
  }

  {
    vgd::Timer t("boost m");
    for (size_t i = 0; i < testNumber; ++i) {
      boost::asio::post(thread_pool2, func_mutex);
    }
  }
  {
    vgd::Timer t("m");
    for (size_t i = 0; i < testNumber; ++i) {
      thread_pool.push_task(func_mutex);
    }
  }

  {
    vgd::Timer t("no m");
    for (size_t i = 0; i < testNumber; ++i) {
      thread_pool.push_task(func_no_mutex);
    }
  }
  {
    vgd::Timer t("boost no m");
    for (size_t i = 0; i < testNumber; ++i) {
      boost::asio::post(thread_pool2, func_no_mutex);
    }
  }

  thread_pool2.join();
  return 0;
}

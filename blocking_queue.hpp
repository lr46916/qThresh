#ifndef _BLOCKING_QUEUE_
#define _BLOCKING_QUEUE_

#include <deque>
#include <condition_variable>
#include <mutex>

// A generic thread-safe STL queue equivalent
//
// Note that `size` and `empty` are not thread-safe (i. e. one thread might alter the contents 
// the moment another thread computes the return value of these methods)
template <typename T>
class BlockingQueue {

private:
  std::mutex mutex;
  std::deque<T> queue;
  std::condition_variable removalCond; // enforces locking on data removal ops

public:
  BlockingQueue();
  BlockingQueue(size_t size) : queue( std::deque<T>(size) ) {};

  bool empty() {
    return this->queue.empty(); 
  }

  size_t size() {
    return this->queue.size();
  }

  void pushBack(T x) {
    std::unique_lock<std::mutex> lock(mutex);
    lock.lock();
    this->queue.push_back(x);
    lock.unlock();
    removalCond.notify_one();
  }

  T popFront() {
    std::unique_lock<std::mutex> lock(mutex);
    lock.lock();
    while (this->queue.empty()) removalCond.wait(lock);
    T front = this->queue.pop_front();
    lock.unlock();
    return front;
  }

};


#endif

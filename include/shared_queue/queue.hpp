#pragma once

#include <mutex>
#include <optional>
#include <queue>


template<typename T>
class SharedQueue
{
 public:
  SharedQueue() {}

  SharedQueue(const SharedQueue& other) : q_(other.q_) {}

  SharedQueue(SharedQueue&& other) : q_(std::move(other.q_)) {}

  template<typename InputIt>
  SharedQueue(InputIt begin, InputIt last)
  {
    std::lock_guard<std::mutex> lock(q_mutex_);
    for (auto it = begin; it != last; ++it)
    {
      q_.push(*it);
    }
  }


  typename std::queue<T>::size_type Size() const
  {
    std::lock_guard<std::mutex> lock(q_mutex_);
    return q_.size();
  }


  void Push(const T& val)
  {
    std::lock_guard<std::mutex> lock(q_mutex_);
    return q_.push(val);
  }

  void Push(T&& val)
  {
    std::lock_guard<std::mutex> lock(q_mutex_);
    return q_.push(val);
  }

  std::optional<T> Pop()
  {
    std::lock_guard<std::mutex> lock(q_mutex_);
    if (q_.empty())
    {
      return std::nullopt;
    }
    T val = std::move(q_.front());
    q_.pop();
    return val;
  }

 private:
  std::queue<T> q_;
  mutable std::mutex q_mutex_;
};
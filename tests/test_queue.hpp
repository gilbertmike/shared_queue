#include <algorithm>
#include <iostream>
#include <thread>

#include "utest/utest.h"

#include "shared_queue/queue.hpp"


UTEST(SharedQueue, OneThread)
{
  SharedQueue<int> queue;
  ASSERT_EQ(0, queue.Size());

  const int N = 10;
  for (int i = 0; i < N; ++i)
  {
    queue.Push(i);
    ASSERT_EQ(i+1, queue.Size());
  }
  for (int i = 0; i < N; ++i)
  {
    std::optional<int> val = queue.Pop();
    ASSERT_TRUE(val);
    ASSERT_EQ(i, *val);
  }
}

UTEST(SharedQueue, IteratorCtor)
{
  std::vector<int> vec = {1, 2, 3, 4, 5};
  SharedQueue<int> queue(vec.begin(), vec.end());
  for (int i = 0; i < vec.size(); ++i)
  {
    auto val = queue.Pop();
    ASSERT_EQ(vec.at(i), *val);
  }
  ASSERT_FALSE(queue.Pop());
}

UTEST(SharedQueue, CopyCtor)
{
  std::vector<int> vec = {1, 2, 3, 4, 5};
  SharedQueue<int> queue(vec.begin(), vec.end());
  SharedQueue<int> copy_queue(queue);

  for (int i = 0; i < vec.size(); ++i)
  {
    auto val = copy_queue.Pop();
    ASSERT_EQ(vec.at(i), *val);
  }
  ASSERT_FALSE(copy_queue.Pop());
}

UTEST(SharedQueue, MoveCtor)
{
  std::vector<int> vec = {1, 2, 3, 4, 5};
  SharedQueue<int> queue(vec.begin(), vec.end());
  SharedQueue<int> moved_queue(std::move(queue));

  for (int i = 0; i < vec.size(); ++i)
  {
    auto val = moved_queue.Pop();
    ASSERT_EQ(vec.at(i), *val);
  }
  ASSERT_FALSE(moved_queue.Pop());
  ASSERT_FALSE(queue.Pop());
}

void PushTask(int n, std::reference_wrapper<SharedQueue<int>> q)
{
  for (int i = 0; i < n; ++i)
  {
    q.get().Push(i);
  }
}

void PopThread(int n,
               std::reference_wrapper<std::vector<int>> vals,
               std::reference_wrapper<SharedQueue<int>> queue)
{
  int count = 0;
  const auto start{std::chrono::steady_clock::now()};
  while (true)
  {
    const auto now{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds(now - start);
    if (elapsed_seconds > std::chrono::seconds(2))
    {
      return;
    }

    std::optional<int> val = queue.get().Pop();
    if (val)
    {
      count += 1;
      vals.get().push_back(*val);
    }
    if (count == n)
    {
      return;
    }
  }
}

UTEST(SharedQueue, OnePushOnePop)
{
  SharedQueue<int> queue;

  std::thread push_thread(PushTask, 5, std::ref(queue));

  std::vector<int> vals;
  std::thread pop_thread(PopThread,
                         5,
                         std::ref(vals),
                         std::ref(queue));

  push_thread.join();
  pop_thread.join();

  for (int i = 0; i < 5; ++i)
  {
    ASSERT_TRUE(std::find(vals.begin(), vals.end(), i) != vals.end());
  }
}

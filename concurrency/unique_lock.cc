#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#define NUM_THREADS 10

volatile int counter = 0;
std::mutex counter_mutex;

void increment(int id) {
  std::unique_lock<std::mutex> lock(counter_mutex, std::defer_lock);
  lock.lock();
  counter++;
  std::cout << id << ": counter = " << counter << std::endl;
  lock.unlock();
}

int main() {
  std::vector<std::thread> threads;

  for (int i = 0; i < NUM_THREADS; i++) {
    threads.emplace_back(&increment, i);
  }

  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }

  return 0;
}

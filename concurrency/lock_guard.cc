#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

volatile int counter = 0;
std::mutex counter_mutex;

void increment(int id) {
  const std::lock_guard<std::mutex> lock(counter_mutex);
  counter++;
  std::cout << id << ": counter = " << counter << std::endl;
}

int main() {
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; i++) {
    threads.push_back(std::thread(&increment, i));
  }

  // wait for threads to finish
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }

  std::cout << "Final counter: " << counter << std::endl;

  return 0;
}

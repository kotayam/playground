#include <iostream>
#include <mutex>
#include <thread>

volatile int counter = 0;
std::mutex counter_mutex;

void increment(int id) {
  const std::lock_guard<std::mutex> lock(counter_mutex);
  counter++;
  std::cout << id << ": counter = " << counter << std::endl;
}

int main() {
  for (int i = 0; i < 10; i++) {
    std::thread(&increment, i);
  }
  return 0;
}

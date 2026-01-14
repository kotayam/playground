#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

volatile int counter = 0;
std::mutex counter_mutex;
std::condition_variable cv;

/**
 * @brief Increment the counter in ascending order of the id by using a
 * condition variable to wait for its turn.
 *
 * @param id
 */
void increment(int id) {
  std::unique_lock lk(counter_mutex);
  // wait until thread's turn
  cv.wait(lk, [id] { return id == counter; });

  // after wait we own the lock
  counter++;
  std::cout << id << ": counter = " << counter << std::endl;

  // release mutex
  lk.unlock();
  // notify all threads to check if its their turn and wake up
  cv.notify_all();
}

int main() {
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; i++) {
    // constructs thread when adding to vector
    threads.emplace_back(&increment, i);
  }

  // wait for threads to complete
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }

  return 0;
}

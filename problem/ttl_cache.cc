/*
Question 3: The Expiring Cache (TTL)
Theme: Background Tasks & Cleanup

Problem Statement: Design a TTLCache where every key has a "Time To Live" in
seconds.

put(key, value, duration): Stores the key. It expires after duration seconds.

get(key): Returns value if not expired. If expired, returns null.

Cleanup: Ensure that expired keys do not consume memory forever.

Constraints:

Thread-safe.

How do you handle cleanup efficiently without blocking the put and get
operations?
*/

/*
Answer: Maintain a priority queue (min heap) to store expiry times (associated
with id), and spawn a thread that checks the queue and cleans up if time.
*/

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

class TTLCache {

  struct CacheEntry {
    std::string value;
    std::chrono::steady_clock::time_point expiry;
  };

  struct QueueEntry {
    std::string key;
    std::chrono::steady_clock::time_point expiry;
  };

  // Use for declaring a comparator for QueueEntry so that it compares based on
  // the expiry field in the priority queue.
  struct CompareEntry {
    bool operator()(const QueueEntry &a, const QueueEntry &b) {
      return a.expiry > b.expiry;
    }
  };

  std::unordered_map<std::string, CacheEntry> cache_;
  std::priority_queue<QueueEntry, std::vector<QueueEntry>, CompareEntry>
      expiryQueue_;
  std::thread bg_;
  mutable std::mutex mtx_;
  std::condition_variable cv_;
  bool end_;

  std::chrono::steady_clock::time_point getCurrentTime() {
    return std::chrono::steady_clock::now();
  }

  void cleanupWorker() {
    std::unique_lock<std::mutex> lock(mtx_);
    while (!end_) {
      if (expiryQueue_.empty()) {
        // nothing to do for now so wait until notified
        cv_.wait(lock, [this] { return end_; });
      }
      std::chrono::steady_clock::time_point curr = getCurrentTime();
      QueueEntry nextExpiry = expiryQueue_.top();
      if (nextExpiry.expiry > curr) {
        // wait until next expiry time
        cv_.wait_until(lock, nextExpiry.expiry);
      } else {
        // remove earliest
        cache_.erase(nextExpiry.key);
        expiryQueue_.pop();
      }
    }
  }

public:
  TTLCache() {
    cache_ = {};
    end_ = false;
    bg_ = std::thread(&TTLCache::cleanupWorker, this);
  }

  ~TTLCache() {
    // join background thread
    end_ = true;
    cv_.notify_all();
    if (bg_.joinable()) {
      bg_.join();
    }
  }

  void put(const std::string &key, const std::string &value, int duration_sec) {
    std::unique_lock<std::mutex> lock(mtx_);
    std::chrono::steady_clock::time_point expiry =
        getCurrentTime() + std::chrono::milliseconds(duration_sec * 1000);
    cache_[key] = {value, expiry};
    expiryQueue_.push({key, expiry});
    lock.unlock();
    // run (unblock) cleanup worker
    cv_.notify_one();
  }

  std::string get(const std::string &key) const {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = cache_.find(key);

    if (it == cache_.end()) {
      return "";
    }
    return it->second.value;
  }
};

int main() {
  TTLCache cache;
  return 0;
}

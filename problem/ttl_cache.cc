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

#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

class TTLCache {
  std::unordered_map<std::string, std::string> cache_;
  std::vector<std::thread> threads_;
  std::thread bg_;
  std::mutex mtx;

  void cleanupWorker(const std::string &key, int duration) {
    sleep(duration);
    std::lock_guard<std::mutex> lock(mtx);
    cache_.erase(key);
  }

  void joinThreads(int joinInterval) {
    while (true) {
      sleep(joinInterval);
      std::unique_lock<std::mutex> lck(mtx);
      for (auto &t : threads_) {
        if (t.joinable()) {
          t.join();
        }
      }
      lck.unlock();
    }
  }

public:
  TTLCache(int joinInterval) {
    cache_ = {};
    threads_ = {};
    // join threads every interval as background job
    bg_ = std::thread(&TTLCache::joinThreads, this, joinInterval);
  }

  ~TTLCache() {
    // join background thread
    if (bg_.joinable()) {
      bg_.join();
    }
  }

  void put(const std::string &key, const std::string &value, int duration) {
    std::lock_guard<std::mutex> lock(mtx);
    cache_.insert(key, value);
    threads_.emplace_back(
        std::thread(&TTLCache::cleanupWorker, this, value, duration));
  }

  std::string *get(const std::string &key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = cache_.find(key);

    if (it == cache_.end()) {
      return nullptr;
    }
    return &it->second;
  }
};

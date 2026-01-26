/*
Question 1: The Snapshot Key-Value Store
Theme: Time-Travel Storage (Core Pure concept)

Problem Statement: Implement a SnapshotMap class that supports the following
operations:

put(key, value): Updates the value of the key.

takeSnapshot(): Returns a snap_id (integer) representing the current state of
the system.

get(key, snap_id): Returns the value of the key at the time of that snapshot. If
the key didn't exist then, return error/null.

Constraints:

The system must be thread-safe.

Optimize for space (do not copy the entire HashMap for every snapshot).

Optimize for fast reads.
*/

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class SnapshotMap {
  struct Entry {
    int snap_id;
    std::string value;
  };
  std::unordered_map<std::string, std::vector<Entry>> db_;
  int curr_snap_id_;
  // mutable allows a member variable to be modified even inside a function that
  // is marked const (get() function in this case).
  mutable std::mutex mtx;

public:
  SnapshotMap() {
    db_ = {};
    curr_snap_id_ = 0;
  }

  void put(const std::string &key, const std::string &value) {
    std::lock_guard<std::mutex> lock(mtx);
    if (db_.find(key) == db_.end()) {
      // insert new vec
      std::vector<Entry> vec = {{curr_snap_id_, value}};
      db_.insert({key, vec});
      return;
    }
    std::vector<Entry> &hist = db_.at(key);
    if (hist.back().snap_id == curr_snap_id_) {
      hist.back().value = value;
    } else {
      hist.push_back({curr_snap_id_, value});
    }
  }

  int takeSnapshot() {
    std::lock_guard<std::mutex> lock(mtx);
    int id = curr_snap_id_;
    curr_snap_id_++;
    return id;
  }

  // we mark this function as const because we should allow const objects to
  // call this function.
  std::string get(const std::string &key, int snap_id) const {
    std::lock_guard<std::mutex> lock(mtx);
    if (db_.find(key) == db_.end()) {
      return "";
    }
    // binary search the entry with snap_id
    const std::vector<Entry> &hist = db_.at(key);
    int l = 0;
    int r = hist.size();
    int m = (r - l) / 2 + l;
    while (l < r) {
      Entry mid = hist[m];
      if (mid.snap_id == snap_id) {
        return mid.value;
      }
      if (snap_id < mid.snap_id) {
        r = m - 1;
      } else {
        l = m + 1;
      }
    }
    return "";
  }
};

int main() {
  SnapshotMap sm;
  return 0;
}

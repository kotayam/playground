/*
Question 2: The "Smart" Resource Allocator
Theme: Low-level Memory Management (Systems)

Problem Statement: Design a class IDAllocator that manages a pool of integer IDs
(e.g., 0 to max_size).

int allocate(): Returns the smallest available non-negative integer.

void release(int id): Releases an ID back into the pool.

bool check(int id): Returns true if the ID is currently allocated.

Constraints:

allocate should be O(log N) or better.

Must be thread-safe.

The system handles millions of IDs (space efficiency matters).
*/

#include <iostream>
#include <mutex>
#include <vector>

class IDAllocator {
  int max_size_;
  std::vector<bool> tree_;
  int leaf_start_idx_;
  mutable std::mutex mtx;

  void updateTree(int idx) {
    while (idx >= 1) {
      int parent = idx / 2;
      int left = 2 * parent;
      int right = 2 * parent + 1;

      bool parentFull = tree_[left] && tree_[right];

      if (tree_[parent] == parentFull) {
        // if parent doesn't change we can stop here
        break;
      }
      tree_[parent] = parentFull;
      idx = parent;
    }
  }

public:
  IDAllocator(int max_size) {
    max_size_ = max_size;
    // use a bit tree where 1 is used, 0 is free
    // index 1 is root
    // left = i * 2
    // right = i * 2 + 1
    tree_.assign(max_size * 2, false);
    leaf_start_idx_ = max_size;
  }

  int allocate() {
    std::lock_guard<std::mutex> lock(mtx);
    if (tree_[1]) {
      return -1;
    }

    int i = 1;
    while (i < leaf_start_idx_) {
      int left = 2 * i;
      int right = 2 * i + 1;
      if (tree_[left]) {
        i = left;
      } else {
        i = right;
      }
    }
    int id = i - leaf_start_idx_;

    tree_[i] = true;
    updateTree(i);

    return id;
  }

  void release(int id) {
    if (id < 0 || id >= max_size_) {
      return;
    }
    int i = id + leaf_start_idx_;

    std::lock_guard<std::mutex> lock(mtx);
    if (!tree_[i])
      return;

    tree_[i] = false;
    updateTree(id);
  }

  bool check(int id) const {
    if (id < 0 || id >= max_size_) {
      return false;
    }

    int i = id + leaf_start_idx_;

    std::lock_guard<std::mutex> lock(mtx);
    return tree_[i];
  }
};

int main() {
  IDAllocator allocator(100);
  std::cout << "Allocated: " << allocator.allocate() << "\n"; // 0
  std::cout << "Allocated: " << allocator.allocate() << "\n"; // 1
  std::cout << "Allocated: " << allocator.allocate() << "\n"; // 2
  std::cout << "Allocated: " << allocator.allocate() << "\n"; // 3
  std::cout << "Allocated: " << allocator.allocate() << "\n"; // -1 (Full)

  std::cout << "Releasing 1...\n";
  allocator.release(1);

  std::cout << "Allocated: " << allocator.allocate() << "\n"; // Should reuse 1
  return 0;
}

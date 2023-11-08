#ifndef ADVENT_OF_CODE_SPLICE_RING_H
#define ADVENT_OF_CODE_SPLICE_RING_H

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "advent_of_code/vlog.h"

namespace advent_of_code {

enum class SpliceRingIndexType {
  kNone = 0,
  kSparse = 1,
  kDense = 2,
};

// Stores a circular buffer with fast random insert and deletes of elements.
// If index_type != kNone, also stores an index for fast lookup of elements.
// For kDense, uses a std::vector indexed by the stored values, while kSparse
// uses an absl::flat_hash_map.
template <typename Storage,
          SpliceRingIndexType index_type = SpliceRingIndexType::kNone>
class SpliceRing {
 public:
  friend class const_iterator;

  // Iterator returned by SpliceRing which supports addition and subtraction as
  // well as (const) value retrieval. Both addition and subtraction support
  // arbitrary addition, but do so through a linear advance through the ring.
  class const_iterator {
   public:
    const_iterator() = default;

    const_iterator& operator++() {
      idx_ = (ring_->list_)[idx_].next;
      return *this;
    }
    const_iterator& operator--() {
      idx_ = (ring_->list_)[idx_].prev;
      return *this;
    }
    const_iterator& operator+=(int64_t num) {
      if (num < 0) return operator-=(-num);
      num %= ring_->size();
      for (int i = 0; i < num; ++i) operator++();
      return *this;
    }
    const_iterator& operator-=(int64_t num) {
      if (num < 0) return operator+=(-num);
      num %= ring_->size();
      for (int i = 0; i < num; ++i) operator--();
      return *this;
    }
    const_iterator operator+(int64_t num) {
      const_iterator ret = *this;
      ret += num;
      return ret;
    }
    const_iterator operator-(int64_t num) {
      const_iterator ret = *this;
      ret -= num;
      return ret;
    }
    const Storage& operator*() const { return (ring_->list_)[idx_].val; }

    bool operator==(const_iterator o) const {
      return ring_ == o.ring_ && idx_ == o.idx_;
    }
    bool operator!=(const_iterator o) const { return !operator==(o); }

   private:
    friend class SpliceRing;
    const_iterator(const SpliceRing* ring, int idx)
     : ring_(ring), idx_(idx) {}
    const SpliceRing* ring_ = nullptr;
    int idx_ = -1;
  };

  SpliceRing() = default;
  SpliceRing(const std::vector<Storage>& init)
   : list_(init.size()), size_(init.size()) {
   for (int i = 0; i < init.size(); ++i) {
     list_[i].val = init[i];
     list_[i].prev = i - 1;
     list_[i].next = i + 1;
   }
   list_[0].prev = list_.size() - 1;
   list_.back().next = 0;
  }

  // Is the ring empty?
  bool empty() const { return size_ == 0; }

  // How many elements are stored within the iteration ring.
  size_t size() const { return size_; }

  // Pre-reserve space to store `size` elements.
  void reserve(size_t size) {
    list_.reserve(size);
  }

  // Inserts the first elemnt in the ring and returns an iterator pointing to
  // that element. Can only be called when the ring is empty (otherwise call
  // InsertAfter or InsertBefore).
  const_iterator InsertFirst(Storage s) {
    CHECK(empty());
    ++size_;
    list_.push_back({.val = std::move(s), .prev = 0, .next = 0});
    if constexpr (index_type == SpliceRingIndexType::kNone) {
      // Nothing to do.
    } else if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_[list_.back().val] = list_.size() - 1;
    } else if constexpr (index_type == SpliceRingIndexType::kDense) {
      if (dense_idx_.size() < list_.back().val + 1) {
        dense_idx_.resize(list_.back().val + 1, -1);
      }
      dense_idx_[list_.back().val] = list_.size() - 1;
    } else {
      LOG(FATAL) << "Unhandled index_type: " << index_type;      
    }
    return const_iterator(this, 0);
  }

  // Inserts `s` after `it` in the ring and returns an iterator pointing to the
  // newly inserted element.
  const_iterator InsertAfter(const_iterator it, Storage s) {
    return InsertBefore(it + 1, std::move(s));
  }

  // Inserts `s` before `it` in the ring and returns an iterator pointing to
  // the newly inserted element.
  const_iterator InsertBefore(const_iterator it, Storage s) {
    ++size_;
    list_.push_back({.val = std::move(s), .prev = list_[it.idx_].prev, .next = it.idx_});

    list_[list_.back().prev].next = list_.size() - 1;
    list_[it.idx_].prev = list_.size() - 1;

    if constexpr (index_type == SpliceRingIndexType::kNone) {
      // Nothing to do.
    } else if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_[list_.back().val] = list_.size() - 1;
    } else if constexpr (index_type == SpliceRingIndexType::kDense) {
      if (dense_idx_.size() < list_.back().val + 1) {
        dense_idx_.resize(list_.back().val + 1, -1);
      }
      dense_idx_[list_.back().val] = list_.size() - 1;
    } else {
      LOG(FATAL) << "Unhandled index_type: " << index_type;
    }
    return const_iterator(this, list_.size() - 1);
  }

  // Returns an iterator pointing to `s` which must have already been added to
  // the ring. Only valid if index_type ~= kNone.
  const_iterator Find(const Storage& s) const {
    static_assert(index_type != SpliceRingIndexType::kNone,
                  "Find does not work without indexing");
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      auto idx_it = sparse_idx_.find(s);
      return const_iterator(this, idx_it->second);
    } else if constexpr (index_type == SpliceRingIndexType::kDense) {
      return const_iterator(this, dense_idx_[s]);
    } else {
      LOG(FATAL) << "Unhandled index_type: " << index_type;
    }
  }

  // Moves the element pointed at by `src_it` to be just before `dest_it`.
  // Returns an iterator pointing at the value one after where src started.
  // `src_it` may have been removed from the ring prior with a call to Remove
  // in which case the return value is undefined.
  const_iterator MoveBefore(const_iterator dest_it, const_iterator src_it) {
    const_iterator ret = src_it;
    ++ret;

    if (list_[src_it.idx_].prev == -1) {
      CHECK_EQ(list_[src_it.idx_].next, -1);
      ++size_;
    } else {
      list_[list_[src_it.idx_].prev].next = list_[src_it.idx_].next;
      list_[list_[src_it.idx_].next].prev = list_[src_it.idx_].prev;
    }

    list_[list_[dest_it.idx_].prev].next = src_it.idx_;
    list_[src_it.idx_].prev = list_[dest_it.idx_].prev;

    list_[dest_it.idx_].prev = src_it.idx_;
    list_[src_it.idx_].next = dest_it.idx_;

    return ret;
  }

  // Removes the elemnt pointed at by `it` from the ring and returns an
  // iterator to the next element in the ring.
  // `it` continues to be valid and can still be used to retrieve the value
  // as well as be re-added with MoveBefore.
  const_iterator Remove(const_iterator it) {
    VLOG(2) << " ...Removing... " << *it;
    --size_;
    if constexpr (index_type == SpliceRingIndexType::kNone) {
      // Nothing to do.
    } else if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_.erase(*it);
    } else if constexpr (index_type == SpliceRingIndexType::kDense) {
      dense_idx_[*it] = -1;
    } else {
      LOG(FATAL) << "Unhandled index_type: " << index_type;
    }

    list_[list_[it.idx_].prev].next = list_[it.idx_].next;
    list_[list_[it.idx_].next].prev = list_[it.idx_].prev;

    const_iterator ret(this, list_[it.idx_].next);

    list_[it.idx_].prev = list_[it.idx_].next = -1;
 
    return ret;
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const SpliceRing& r,
                            int limit = 30) {
    if (r.empty()) {
      sink.Append("()");
      return;
    }
    auto start = r.FirstAdded();
    absl::Format(&sink, "(%v", *start);
    for (auto it = start + 1; it != start; ++it) {
      if (--limit == 0) {
        sink.Append(",...");
        break;
      }
      absl::Format(&sink, ",%v", *it);
    }
    sink.Append(")");
  }

 private:
  struct DoubleLinkedList {
    Storage val;
    int prev;
    int next;
  };

  const_iterator FirstAdded() const {
    for (int i = 0; i < list_.size(); ++i) {
      if (list_[i].prev != -1) {
        return const_iterator(this, i);
      }
    }
    return const_iterator(this, -1);
  }

  std::vector<DoubleLinkedList> list_;
  size_t size_ = 0;

  absl::flat_hash_map<Storage, int> sparse_idx_;
  std::vector<int> dense_idx_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_SPLICE_RING_H
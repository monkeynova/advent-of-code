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

template <typename Storage,
          SpliceRingIndexType index_type = SpliceRingIndexType::kNone>
class SpliceRing {
 public:
  friend class const_iterator;

  struct DoubleLinkedList {
    Storage val;
    int prev;
    int next;
  };

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
    const_iterator& operator+=(int num) {
      if (num < 0) return operator-=(-num);
      num %= ring_->size();
      for (int i = 0; i < num; ++i) operator++();
      return *this;
    }
    const_iterator& operator-=(int num) {
      if (num < 0) return operator+=(-num);
      num %= ring_->size();
      for (int i = 0; i < num; ++i) operator--();
      return *this;
    }
    const_iterator operator+(int num) {
      const_iterator ret = *this;
      ret += num;
      return ret;
    }
    const_iterator operator-(int num) {
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

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }
  void reserve(size_t size) {
    list_.reserve(size);
  }

  const_iterator InsertFirst(Storage s) {
    ++size_;
    CHECK(list_.empty());
    list_.push_back({.val = std::move(s), .prev = 0, .next = 0});
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_[list_.back().val] = 0;
    }
    if constexpr (index_type == SpliceRingIndexType::kDense) {
      if (dense_idx_.size() < list_.back().val + 1) {
        dense_idx_.resize(list_.back().val + 1, -1);
      }
      dense_idx_[list_.back().val] = 0;
    }
    return const_iterator(this, 0);
  }
  void InsertBefore(const_iterator it, Storage s) {
    ++size_;
    list_.push_back({.val = std::move(s), .prev = -1, .next = -1});

    list_.back().prev = list_[it.idx_].prev;
    list_[list_.back().prev].next = list_.size() - 1;

    list_.back().next = it.idx_;
    list_[it.idx_].prev = list_.size() - 1;

    if constexpr (index_type == SpliceRingIndexType::kNone) {
      return;
    }
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_[list_.back().val] = list_.size() - 1;
    }
    if constexpr (index_type == SpliceRingIndexType::kDense) {
      if (dense_idx_.size() < list_.back().val + 1) {
        dense_idx_.resize(list_.back().val + 1, -1);
      }
      dense_idx_[list_.back().val] = list_.size() - 1;
    }
  }
  const_iterator Find(const Storage& s) const {
    static_assert(index_type != SpliceRingIndexType::kNone,
                  "Find does not work without indexing");
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      auto idx_it = sparse_idx_.find(s);
      return const_iterator(this, idx_it->second);
    }
    if constexpr (index_type == SpliceRingIndexType::kDense) {
      return const_iterator(this, dense_idx_[s]);
    }
    // Deliberate fall through to produce a compile error for new type.
  }
  const_iterator MoveBefore(const_iterator dest_it, const_iterator src_it) {
    if (list_[src_it.idx_].prev == -1) {
      CHECK_EQ(list_[src_it.idx_].next, -1);
      ++size_;
    }
    const_iterator ret = src_it;
    ++ret;

    list_[list_[src_it.idx_].prev].next = list_[src_it.idx_].next;
    list_[list_[src_it.idx_].next].prev = list_[src_it.idx_].prev;

    list_[list_[dest_it.idx_].prev].next = src_it.idx_;
    list_[src_it.idx_].prev = list_[dest_it.idx_].prev;

    list_[dest_it.idx_].prev = src_it.idx_;
    list_[src_it.idx_].next = dest_it.idx_;

    return ret;
  }

  const_iterator Erase(const_iterator it) {
    --size_;
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_.erase(*it);
    }
    if constexpr (index_type == SpliceRingIndexType::kDense) {
      dense_idx_[*it] = -1;
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
    auto it = r.FirstAdded();
    auto start = it;
    absl::Format(&sink, "(%v", *it);
    for (++it; it != start; ++it) {
      if (--limit == 0) {
        sink.Append(",...");
        break;
      }
      absl::Format(&sink, ",%v", *it);
    }
    sink.Append(")");
  }

 private:
  const_iterator FirstAdded() const {
    return const_iterator(this, 0);
  }

  std::vector<DoubleLinkedList> list_;
  int size_ = 0;

  absl::flat_hash_map<Storage, int> sparse_idx_;
  std::vector<int> dense_idx_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_SPLICE_RING_H
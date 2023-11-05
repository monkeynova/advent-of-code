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
  struct DoubleLinkedList {
    Storage val;
    int prev;
    int next;
  };

  class const_iterator {
   public:
    const_iterator() = default;

    const_iterator& operator++() {
      idx_ = (*list_)[idx_].next;
      return *this;
    }
    const_iterator& operator--() {
      idx_ = (*list_)[idx_].prev;
      return *this;
    }
    const_iterator& operator+=(int num) {
      if (num < 0) return operator-=(-num);
      for (int i = 0; i < num; ++i) operator++();
      return *this;
    }
    const_iterator& operator-=(int num) {
      if (num < 0) return operator+=(-num);
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
    const Storage& operator*() const { return (*list_)[idx_].val; }

    bool operator==(const_iterator o) const {
      return list_ == o.list_ && idx_ == o.idx_;
    }
    bool operator!=(const_iterator o) const { return !operator==(o); }

   private:
    friend class SpliceRing;
    const_iterator(const std::vector<DoubleLinkedList>* list, int idx)
     : list_(list), idx_(idx) {}
    const std::vector<DoubleLinkedList>* list_ = nullptr;
    int idx_ = -1;
  };

  SpliceRing() = default;

  bool empty() const { return list_.empty(); }
  size_t size() const { return list_.size(); }
  void reserve(size_t size) {
    list_.reserve(size);
  }

  void InsertFirst(Storage s) {
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
  }
  void InsertBefore(const_iterator it, Storage s) {
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
  const_iterator SomePoint() const {
    return const_iterator(&list_, 0);
  }
  const_iterator Find(const Storage& s) const {
    static_assert(index_type != SpliceRingIndexType::kNone,
                  "Find does not work without indexing");
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      auto idx_it = sparse_idx_.find(s);
      return const_iterator(&list_, idx_it->second);
    }
    if constexpr (index_type == SpliceRingIndexType::kDense) {
      return const_iterator(&list_, dense_idx_[s]);
    }
    // Deliberate fall through to produce a compile error for new type.
  }
  const_iterator MoveBefore(const_iterator dest_it, const_iterator src_it) {
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
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_.erase(*it);
    }
    list_[list_[it.idx_].prev].next = list_[it.idx_].next;
    list_[list_[it.idx_].next].prev = list_[it.idx_].prev;
    return const_iterator(&list_, list_[it.idx_].next);
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const SpliceRing& r) {
    absl::Format(
        &sink, "%s",
        absl::StrJoin(
            r.list_, ",",
            [](std::string* out, const DoubleLinkedList& dll) {
                absl::StrAppendFormat(out, "%v", dll.val);
            }));
  }

 private:
  std::vector<DoubleLinkedList> list_;

  absl::flat_hash_map<Storage, int> sparse_idx_;
  std::vector<int> dense_idx_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_SPLICE_RING_H
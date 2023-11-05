#ifndef ADVENT_OF_CODE_SPLICE_RING_H
#define ADVENT_OF_CODE_SPLICE_RING_H

#include <list>

#include "advent_of_code/vlog.h"

namespace advent_of_code {

enum class SpliceRingIndexType {
  kNone = 0,
  kSparse = 1,
};

template <typename Storage,
          SpliceRingIndexType index_type = SpliceRingIndexType::kNone>
class SpliceRing {
 public:
  class const_iterator {
   public:
    const_iterator() = default;

    const_iterator& operator++() {
      ++it_;
      if (it_ == list_->end()) it_ = list_->begin();
      return *this;
    }
    const_iterator& operator--() {
      if (it_ == list_->begin()) it_ = list_->end();
      --it_;
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
    const Storage& operator*() const { return *it_; }

    bool operator==(const_iterator o) const {
      return list_ == o.list_ && it_ == o.it_;
    }
    bool operator!=(const_iterator o) const { return !operator==(o); }

   private:
    friend class SpliceRing;
    const_iterator(const std::list<Storage>* list,
                   typename std::list<Storage>::const_iterator it)
     : list_(list), it_(it) {}
    const std::list<Storage>* list_ = nullptr;
    typename std::list<Storage>::const_iterator it_;
  };

  SpliceRing() = default;

  bool empty() const { return list_.empty(); }
  size_t size() const { return list_.size(); }

  void InsertSomewhere(Storage s) {
    list_.push_back(std::move(s));
    if constexpr (index_type == SpliceRingIndexType::kNone) {
      return;
    }
    ListIterator it = list_.end();
    --it;
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_[*it] = it;
    }
  }
  void InsertBefore(const_iterator it, Storage s) {
    list_.insert(it.it_, std::move(s));
    if constexpr (index_type == SpliceRingIndexType::kNone) {
      return;
    }
    ListIterator to_idx = it.it_;
    --to_idx;
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_[*to_idx] = to_idx;
    }
  }
  const_iterator SomePoint() const {
    return const_iterator(&list_, list_.begin());
  }
  const_iterator Find(const Storage& s) const {
    static_assert(index_type != SpliceRingIndexType::kNone,
                  "Find does not work without indexing");
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      auto idx_it = sparse_idx_.find(s);
      return const_iterator(&list_, idx_it->second);
    }
    // Deliberate fall through to produce a compile error for new type.
  }
  const_iterator MoveBefore(const_iterator dest_it, const_iterator src_it) {
    auto src_next = src_it.it_;
    ++src_next;
    list_.splice(dest_it.it_, list_, src_it.it_, src_next);
    if (src_next == list_.end()) src_next = list_.begin();
    return const_iterator(&list_, src_next);
  }

  const_iterator Erase(const_iterator it) {
    if constexpr (index_type == SpliceRingIndexType::kSparse) {
      sparse_idx_.erase(*it);
    }
    auto list_it = list_.erase(it.it_);
    if (list_it == list_.end()) list_it = list_.begin();
    return const_iterator(&list_, list_it);
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const SpliceRing& r) {
    absl::Format(&sink, "%s", absl::StrJoin(r.list_, ","));
  }

 private:
  using ListIterator = typename std::list<Storage>::const_iterator;

  std::list<Storage> list_;
  absl::flat_hash_map<Storage, ListIterator> sparse_idx_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_SPLICE_RING_H
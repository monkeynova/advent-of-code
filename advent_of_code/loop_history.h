#ifndef ADVENT_OF_CODE_LOOP_HISTORY_H
#define ADVENT_OF_CODE_LOOP_HISTORY_H

#include <cstdint>
#include <vector>

#include "absl/container/node_hash_map.h"

namespace advent_of_code {

template <typename Storage>
class LoopHistory {
 public:
  bool AddMaybeNew(Storage s) {
    if (loop_size_ != -1) return true;

    auto [it, inserted] = hist_.emplace(std::move(s), idx_.size());
    if (inserted) {
      idx_.push_back(&it->first);
      return false;
    }
    loop_size_ = idx_.size() - it->second;
    loop_offset_ = it->second;
    return true;
  }

  int64_t CyclesTo(int64_t offset) {
    return (offset - loop_offset_) / loop_size_;
  }
  int64_t IndexAt(int64_t offset) {
    return (offset - loop_offset_) % loop_size_ + loop_offset_;
  }

  const Storage& FindInLoop(int64_t offset) { return *idx_[IndexAt(offset)]; }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const LoopHistory& l) {
    absl::Format(&sink, "Loop [%d->%d)", l.loop_offset_,
                 l.loop_offset_ + l.loop_size_);
  }

 private:
  absl::node_hash_map<Storage, int> hist_;
  std::vector<const Storage*> idx_;
  int loop_size_ = -1;
  int loop_offset_ = -1;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_LOOP_HISTORY_H
#ifndef ADVENT_OF_CODE_BFS_H
#define ADVENT_OF_CODE_BFS_H

#include <deque>
#include <memory>
#include <vector>

#include "glog/logging.h"

namespace advent_of_code {

template <typename BFSImpl>
class BFSInterface {
 public:
  class State {
   public:
    explicit State(BFSImpl start) {
      hist_.insert(start);
      frontier_.push_back(std::move(start));
    }
    void AddNextStep(BFSImpl next) {
      ++next.num_steps_;
      if (next.IsFinal()) ret = next.num_steps_;
      if (hist_.contains(next)) return;
      hist_.insert(next);
      frontier_.push_back(std::move(next));
    }

   private:
    std::deque<BFSImpl> frontier_;
    absl::flat_hash_set<BFSImpl> hist_;
    absl::optional<int> ret;

    friend class BFSInterface;
  };

  virtual ~BFSInterface() = default;
  virtual void AddNextSteps(State* state) = 0;
  virtual bool IsFinal() = 0;

  absl::optional<int> FindMinSteps();

 protected:
  BFSInterface() = default;

 private:
  int num_steps_ = 0;

  friend class State;
};

template <typename BFSImpl>
absl::optional<int> BFSInterface<BFSImpl>::FindMinSteps() {
  static_assert(std::is_base_of<BFSInterface<BFSImpl>, BFSImpl>(),
                "BFSInterface must be templated with a subclass");
  State state(*dynamic_cast<BFSImpl*>(this));
  while (!state.frontier_.empty()) {
    BFSImpl& cur = state.frontier_.front();
    VLOG(3) << "@" << cur.num_steps_ << "; " << cur;
    cur.AddNextSteps(&state);
    if (state.ret) return *state.ret;
    state.frontier_.pop_front();
  }
  return absl::nullopt;
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_BFS_H
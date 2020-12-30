#ifndef ADVENT_OF_CODE_BFS_H
#define ADVENT_OF_CODE_BFS_H

#include <deque>
#include <memory>
#include <queue>
#include <vector>

#include "glog/logging.h"

namespace advent_of_code {

template <typename BFSImpl>
class AStarGT {
 public:
  bool operator()(const BFSImpl& a, const BFSImpl& b) {
    return a.num_steps_ + a.min_steps_to_final() >
      b.num_steps_ + b.min_steps_to_final();
  }
};

template <typename T>
struct BFSInterfaceTraits { using RefType = const T&; };

template <>
struct BFSInterfaceTraits<absl::string_view> { using RefType = absl::string_view; };

struct Point;

template <>
struct BFSInterfaceTraits<Point> { using RefType = Point; };

template <typename BFSImpl, typename HistType = BFSImpl>
class BFSInterface {
 public:
  class State;

  virtual ~BFSInterface() = default;

  virtual typename BFSInterfaceTraits<HistType>::RefType identifier() const = 0;
  virtual void AddNextSteps(State* state) = 0;
  virtual bool IsFinal() = 0;

  virtual int min_steps_to_final() const { return 0; }

  absl::optional<int> FindMinSteps();
  absl::optional<int> FindMinStepsAStar();

 protected:
  BFSInterface() = default;

  int num_steps() const { return num_steps_; }

 private:
  int num_steps_ = 0;

  friend class State;
  friend class AStarGT<BFSImpl>;
};

template <typename BFSImpl, typename HistType>
class BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit State(const BFSImpl& start) {
    hist_.insert(start.identifier());
  }

  void AddNextStep(BFSImpl next) {
    ++next.num_steps_;
    if (next.IsFinal()) ret = next.num_steps_;
    if (hist_.contains(next.identifier())) return;
    hist_.insert(next.identifier());
    AddToFrontier(std::move(next));
  }

  virtual void AddToFrontier(BFSImpl next) = 0;

 private:
  absl::flat_hash_set<HistType> hist_;
  absl::optional<int> ret;
  friend class BFSInterface;
};

template <typename BFSImpl, typename HistType>
class DequeState : public BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit DequeState(BFSImpl start)
  : BFSInterface<BFSImpl, HistType>::State(start) { AddToFrontier(start); }

  void AddToFrontier(BFSImpl next) final { frontier_.push_back(std::move(next)); }

 private:
  std::deque<BFSImpl> frontier_;
  friend class BFSInterface<BFSImpl, HistType>;
};

template <typename BFSImpl, typename HistType>
class QueueState : public BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit QueueState(BFSImpl start)
   : BFSInterface<BFSImpl, HistType>::State(start) { AddToFrontier(start); }

  void AddToFrontier(BFSImpl next) final { frontier_.push(std::move(next)); }

 private:
  std::priority_queue<BFSImpl, std::vector<BFSImpl>, AStarGT<BFSImpl>> frontier_;
  friend class BFSInterface<BFSImpl, HistType>;
};

template <typename BFSImpl, typename HistType>
absl::optional<int> BFSInterface<BFSImpl, HistType>::FindMinSteps() {
  static_assert(std::is_base_of<BFSInterface<BFSImpl, HistType>, BFSImpl>(),
                "BFSInterface must be templated with a subclass");
  DequeState<BFSImpl, HistType> state(*dynamic_cast<BFSImpl*>(this));
  while (!state.frontier_.empty()) {
    BFSImpl& cur = state.frontier_.front();
    VLOG(3) << "@" << cur.num_steps_ << "; " << cur.identifier();
    cur.AddNextSteps(&state);
    if (state.ret) return *state.ret;
    state.frontier_.pop_front();
  }
  return absl::nullopt;
}

template <typename BFSImpl, typename HistType>
absl::optional<int> BFSInterface<BFSImpl, HistType>::FindMinStepsAStar() {
  static_assert(std::is_base_of<BFSInterface<BFSImpl, HistType>, BFSImpl>(),
                "BFSInterface must be templated with a subclass");
  QueueState<BFSImpl, HistType> state(*dynamic_cast<BFSImpl*>(this));
  while (!state.frontier_.empty()) {
    BFSImpl cur = state.frontier_.top();
    state.frontier_.pop();
    VLOG(3) << "@" << cur.num_steps_ << "; " << cur.identifier();
    cur.AddNextSteps(&state);
    if (state.ret) return *state.ret;
  }
  return absl::nullopt;
}


}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_BFS_H
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
    // Tie break with num_steps sp we can't add to the history a backtrack.
    int a_steps = a.num_steps_ + a.min_steps_to_final();
    int b_steps = b.num_steps_ + b.min_steps_to_final();
    if (a_steps != b_steps) return a_steps > b_steps;
    return a.num_steps_ > b.num_steps_;
  }
};

template <typename T>
struct BFSInterfaceTraits {
  using RefType = const T&;
};

template <>
struct BFSInterfaceTraits<absl::string_view> {
  using RefType = absl::string_view;
};

struct Point;

template <>
struct BFSInterfaceTraits<Point> {
  using RefType = Point;
};

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

  int num_steps() const { return num_steps_; }

 protected:
  BFSInterface() = default;

  void add_steps(int num_steps) { num_steps_ += num_steps; }

 private:
  int num_steps_ = 0;

  friend class State;
  friend class AStarGT<BFSImpl>;
};

template <typename BFSImpl, typename HistType>
class BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit State(const BFSImpl& start) { hist_[start.identifier()] = 0; }

  void AddNextStep(BFSImpl next) {
    ++next.num_steps_;
    if (next.IsFinal()) ret = next.num_steps_;
    auto it = hist_.find(next.identifier());
    if (it == hist_.end() || it->second > next.num_steps()) {
      hist_[next.identifier()] = next.num_steps();
      AddToFrontier(std::move(next));
    }
  }

  virtual void AddToFrontier(BFSImpl next) = 0;

 private:
  absl::flat_hash_map<HistType, int> hist_;
  absl::optional<int> ret;
  friend class BFSInterface;
};

template <typename BFSImpl, typename HistType>
class DequeState : public BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit DequeState(BFSImpl start)
      : BFSInterface<BFSImpl, HistType>::State(start) {
    AddToFrontier(start);
  }

  void AddToFrontier(BFSImpl next) final {
    frontier_.push_back(std::move(next));
  }

 private:
  std::deque<BFSImpl> frontier_;
  friend class BFSInterface<BFSImpl, HistType>;
};

template <typename BFSImpl, typename HistType>
class QueueState : public BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit QueueState(BFSImpl start)
      : BFSInterface<BFSImpl, HistType>::State(start) {
    AddToFrontier(start);
  }

  void AddToFrontier(BFSImpl next) final {
    VLOG(4) << "    Add: " << next.identifier() << " (" << next.num_steps() 
            << "+" << next.min_steps_to_final() << ")";
    frontier_.push(std::move(next));
  }

 private:
  std::priority_queue<BFSImpl, std::vector<BFSImpl>, AStarGT<BFSImpl>>
      frontier_;
  friend class BFSInterface<BFSImpl, HistType>;
};

template <typename BFSImpl, typename HistType>
absl::optional<int> BFSInterface<BFSImpl, HistType>::FindMinSteps() {
  static_assert(std::is_base_of<BFSInterface<BFSImpl, HistType>, BFSImpl>(),
                "BFSInterface must be templated with a subclass");
  if (IsFinal()) return 0;
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
  VLOG(3) << "New AStar: " << identifier();
  if (IsFinal()) return 0;
  QueueState<BFSImpl, HistType> state(*dynamic_cast<BFSImpl*>(this));
  while (!state.frontier_.empty()) {
    BFSImpl cur = state.frontier_.top();
    if (cur.IsFinal()) return cur.num_steps();
    VLOG(3) << "  Next: " << cur.identifier() << " (" << cur.num_steps() << "+"
            << cur.min_steps_to_final() << ")";
    state.frontier_.pop();
    cur.AddNextSteps(&state);
    //if (state.ret) return *state.ret;
  }
  return absl::nullopt;
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_BFS_H
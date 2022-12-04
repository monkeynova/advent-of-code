#ifndef ADVENT_OF_CODE_BFS_H
#define ADVENT_OF_CODE_BFS_H

#include <deque>
#include <memory>
#include <queue>
#include <vector>

#include "absl/log/log.h"

namespace advent_of_code {

template <typename BFSImpl>
class AStarGT {
 public:
  bool operator()(const BFSImpl& a, const BFSImpl& b) {
    // Tie break with num_steps sp we can't add to the history a backtrack.
    int a_steps = a.num_steps() + a.min_steps_to_final();
    int b_steps = b.num_steps() + b.min_steps_to_final();
    if (a_steps != b_steps) return a_steps > b_steps;
    return a.num_steps() > b.num_steps();
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

struct Point3;
template <>
struct BFSInterfaceTraits<Point3> {
  using RefType = Point3;
};

template <typename BFSImpl, typename HistType = BFSImpl>
class BFSInterface {
 public:
  class State;
  class DequeState;
  class QueueState;

  virtual ~BFSInterface() = default;

  virtual typename BFSInterfaceTraits<HistType>::RefType identifier() const = 0;
  virtual void AddNextSteps(State* state) const = 0;
  virtual bool IsFinal() const = 0;

  virtual int min_steps_to_final() const { return 0; }

  absl::optional<int> FindMinSteps() { return FindMinStepsImpl<DequeState>(); }
  absl::optional<int> FindMinStepsAStar() {
    static_assert(std::is_copy_assignable_v<BFSImpl>,
                  "BFSImpl must be copy-assignable for AStar");
    VLOG(3) << "New AStar: " << identifier();
    return FindMinStepsImpl<QueueState>();
  }

  int num_steps() const { return num_steps_; }

 protected:
  BFSInterface() = default;

  void add_steps(int num_steps) { num_steps_ += num_steps; }

 private:
  template <typename QueueType>
  absl::optional<int> FindMinStepsImpl() {
    static_assert(std::is_base_of<BFSInterface<BFSImpl, HistType>, BFSImpl>(),
                  "BFSInterface must be templated with a subclass");
    if (IsFinal()) return 0;
    BFSImpl& start = *dynamic_cast<BFSImpl*>(this);
    for (QueueType state(start); !state.empty(); state.pop()) {
      const BFSImpl& cur = state.next();
      if (cur.IsFinal()) return cur.num_steps();
      VLOG(3) << "@" << cur.num_steps_ << "; " << cur.identifier();
      cur.AddNextSteps(&state);
      if (state.ret()) return *state.ret();
    }
    return absl::nullopt;
  }

  int num_steps_ = 0;
};

template <typename BFSImpl, typename HistType>
class BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit State(const BFSImpl& start) { hist_[start.identifier()] = 0; }

  void AddNextStep(BFSImpl next) {
    ++next.num_steps_;
    auto it = hist_.find(next.identifier());
    if (it != hist_.end() && it->second <= next.num_steps()) return;
    hist_[next.identifier()] = next.num_steps();
    AddToFrontier(std::move(next));
  }

  virtual void AddToFrontier(BFSImpl next) = 0;

  absl::optional<int> ret() const { return ret_; }

 protected:
  void set_ret(int ret) { ret_ = ret; }

 private:
  absl::flat_hash_map<HistType, int> hist_;
  absl::optional<int> ret_;
};

template <typename BFSImpl, typename HistType>
class BFSInterface<BFSImpl, HistType>::DequeState
    : public BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit DequeState(BFSImpl start)
      : BFSInterface<BFSImpl, HistType>::State(start) {
    AddToFrontier(std::move(start));
  }

  void AddToFrontier(BFSImpl next) final {
    if (next.IsFinal()) this->set_ret(next.num_steps());
    frontier_.push_back(std::move(next));
  }

  bool empty() const { return frontier_.empty(); }
  const BFSImpl& next() const { return frontier_.front(); }
  void pop() { return frontier_.pop_front(); }

 private:
  std::deque<BFSImpl> frontier_;
};

template <typename BFSImpl, typename HistType>
class BFSInterface<BFSImpl, HistType>::QueueState
    : public BFSInterface<BFSImpl, HistType>::State {
 public:
  explicit QueueState(BFSImpl start)
      : BFSInterface<BFSImpl, HistType>::State(start) {
    AddToFrontier(std::move(start));
  }

  void AddToFrontier(BFSImpl next) final {
    VLOG(4) << "    Add: " << next.identifier() << " (" << next.num_steps()
            << "+" << next.min_steps_to_final() << ")";
    frontier_.push(std::move(next));
  }

  bool empty() const { return frontier_.empty(); }
  BFSImpl next() const { return frontier_.top(); }
  void pop() { return frontier_.pop(); }

 private:
  std::priority_queue<BFSImpl, std::vector<BFSImpl>, AStarGT<BFSImpl>>
      frontier_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_BFS_H
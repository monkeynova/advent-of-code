#ifndef ADVENT_OF_CODE_BFS_H
#define ADVENT_OF_CODE_BFS_H

#include <deque>
#include <memory>
#include <queue>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/log.h"

namespace advent_of_code {

// General implementation of a breadth-first-search algorithm. The class uses
// the curiously recurring template pattern
// (https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern) in
// order to build a base class that can store a container of the specific
// sub-class in order to queue states to explore.
//
// Expected usage is to subclass BFSInterface templated on a representation of
// a state during the search. An optional second parameter is used to store
// a history of visited states, and may be used if, for example, there is
// signficant redundany in the state (like, say a reference to a CharBoard
// where the relevant state is simply a Point).
//
// State subclasses are expected to override the abstract methods in order
// to perform the search.
template <typename BFSImpl, typename HistType = const BFSImpl&>
class BFSInterface {
 public:
  class State;
  class DequeState;
  class QueueState;

  virtual ~BFSInterface() = default;

  // Returns a historical identifier. This type must have a valid AbslHash
  // implementation.
  virtual HistType identifier() const = 0;

  // When called, should add to `state` all rechable forms of BFSImpl from
  // `this` (for example all Points reachable on the CharBoard from the
  // current location).
  virtual void AddNextSteps(State* state) const = 0;

  // Must return true if and only if the desired breadth-first search is
  // complete in `this` (e.g. the we've reached the desired end Point).
  virtual bool IsFinal() const = 0;

  // Controls the behavior of FindMinStepsAStar(). It must be the case that any
  // path from `this` to a state returning true in IsFinal must take at least
  // this man steps. For example (Point{end} - Point{start}).dist() would
  // return the minimal (unobstructed) distance from the end point and meet the
  // requirements.
  virtual int min_steps_to_final() const { return 0; }

  // Implements a standard breadth-first search from `this` to a subsequent
  // BFSImpl such that IsFinal returns true and returns the number of steps
  // required to go from `this` to the final state (or absl::nullopt if such a
  // state is unreachable).
  absl::optional<int> FindMinSteps() {
    DequeState state;
    return FindMinStepsImpl(state);
  }

  // Performs the same breadth-first search as `FindMinSteps`, but rather than
  // returning the lenght of the shortest path, returns a map of all reachable
  // identifiers to the shortest path length to reach that identifier. It is
  // generally expected that calls to this are on instances where IsFinal
  // always returns false.
  absl::flat_hash_map<std::remove_reference_t<HistType>, int> FindReachable() {
    DequeState state;
    (void)FindMinStepsImpl(state);
    return state.ConsumeHist();
  }

  // Performs the same breadth-first search as `FindMinSteps`, but rather than
  // returning the lenght of the shortest path, returns nothing. It is
  // generally expected that calls to this are doing additional tracking in
  // their callbacks.
  void Walk() {
    DequeState state;
    (void)FindMinStepsImpl(state);
  }

  // Implements an A* algorithm search like `FindMinSteps` where
  // `min_steps_to_final` is used to prioitize the search space. For example,
  // if min_steps_to_final where to return (Point{end} - Point{start}).dist()
  // the search space would prioritize more direct paths over a fully breadth
  // first search space.
  absl::optional<int> FindMinStepsAStar() {
    static_assert(std::is_copy_assignable_v<BFSImpl>,
                  "BFSImpl must be copy-assignable for AStar");
    VLOG(3) << "New AStar: " << identifier();
    QueueState state;
    return FindMinStepsImpl(state);
  }

  int num_steps() const { return num_steps_; }

 protected:
  BFSInterface() = default;

  // In the event that going from `this` to an entry added by AddNextSteps
  // needs to account for more than a single action, add_steps can adjust the
  // distance.
  // If used, `FindMinSteps` will not return an accurate answer and
  // `FindMinStepsAStar` must be used instead.
  void add_steps(int num_steps) { num_steps_ += num_steps; }

 private:
  template <typename QueueType>
  absl::optional<int> FindMinStepsImpl(QueueType& state) {
    static_assert(std::is_base_of<BFSInterface<BFSImpl, HistType>, BFSImpl>(),
                  "BFSInterface must be templated with a subclass");
    if (IsFinal()) return 0;
    BFSImpl& start = *dynamic_cast<BFSImpl*>(this);
    for (state.Start(start); !state.empty(); state.pop()) {
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

// Stores the queue of work for performing a breadth-first search. The base
// State class stores the history while the specific sub classes store the
// implementation of the queuing behavior.
template <typename BFSImpl, typename HistType>
class BFSInterface<BFSImpl, HistType>::State {
 public:
  State() = default;

  void Start(BFSImpl start) {
    CHECK(hist_.empty());
    hist_.emplace(start.identifier(), 0);
    AddToFrontier(std::move(start));
  }

  // Adds `next` to the processing queue if it is not present in `hist_`.
  // next.num_steps() is incremented in the process.
  void AddNextStep(BFSImpl next) {
    ++next.num_steps_;
    auto it = hist_.find(next.identifier());
    if (it != hist_.end() && it->second <= next.num_steps()) return;
    hist_[next.identifier()] = next.num_steps();
    AddToFrontier(std::move(next));
  }

  absl::optional<int> ret() const { return ret_; }

  absl::flat_hash_map<std::remove_reference_t<HistType>, int> ConsumeHist() {
    return std::move(hist_);
  }

 protected:
  virtual void AddToFrontier(BFSImpl next) = 0;

  void set_ret(int ret) { ret_ = ret; }

 private:
  absl::flat_hash_map<std::remove_reference_t<HistType>, int> hist_;
  absl::optional<int> ret_;
};

// Uses a std::deque to process work, so all states are processed in a FIFO
// manner.
template <typename BFSImpl, typename HistType>
class BFSInterface<BFSImpl, HistType>::DequeState
    : public BFSInterface<BFSImpl, HistType>::State {
 public:
  DequeState() : BFSInterface<BFSImpl, HistType>::State() {}

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

// Uses a std::priority_queue to process work, so all states are processed
// ordered by num_steps() + min_steps_to_final().
template <typename BFSImpl, typename HistType>
class BFSInterface<BFSImpl, HistType>::QueueState
    : public BFSInterface<BFSImpl, HistType>::State {
 public:
  QueueState() : BFSInterface<BFSImpl, HistType>::State() {}

  void AddToFrontier(BFSImpl next) final {
    VLOG(4) << "    Add: " << next.identifier() << " (" << next.num_steps()
            << "+" << next.min_steps_to_final() << ")";
    frontier_.push(std::move(next));
  }

  bool empty() const { return frontier_.empty(); }
  BFSImpl next() const { return frontier_.top(); }
  void pop() { return frontier_.pop(); }

 private:
  class AStarGT {
   public:
    bool operator()(const BFSImpl& a, const BFSImpl& b) {
      // Tie break with num_steps so we can't add to the history a backtrack.
      int a_steps = a.num_steps() + a.min_steps_to_final();
      int b_steps = b.num_steps() + b.min_steps_to_final();
      if (a_steps != b_steps) return a_steps > b_steps;
      return a.num_steps() > b.num_steps();
    }
  };

  std::priority_queue<BFSImpl, std::vector<BFSImpl>, AStarGT> frontier_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_BFS_H
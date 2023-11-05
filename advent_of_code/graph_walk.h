#ifndef ADVENT_OF_CODE_GRAPH_WALK_H
#define ADVENT_OF_CODE_GRAPH_WALK_H

#include "absl/functional/function_ref.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/directed_graph.h"

namespace advent_of_code {

class GraphWalk : public BFSInterface<GraphWalk, absl::string_view> {
 public:
  struct Options {
    const DirectedGraphBase* graph;
    absl::string_view start;
    absl::FunctionRef<bool(absl::string_view, int)> is_good;
    absl::FunctionRef<bool(absl::string_view, int)> is_final;
  };

  GraphWalk(Options options)
      : is_good_(options.is_good),
        is_final_(options.is_final),
        graph_(options.graph),
        cur_(options.start) {}

  absl::string_view identifier() const override { return cur_; }
  bool IsFinal() const override { return is_final_(cur_, num_steps()); }
  void AddNextSteps(State* state) const override {
    const std::vector<absl::string_view>* outgoing = graph_->Outgoing(cur_);
    if (outgoing == nullptr) return;
    for (absl::string_view test : *outgoing) {
      if (!is_good_(test, num_steps() + 1)) continue;
      GraphWalk next = *this;
      next.cur_ = test;
      state->AddNextStep(next);
    }
  }

 private:
  absl::FunctionRef<bool(absl::string_view, int)> is_good_;
  absl::FunctionRef<bool(absl::string_view, int)> is_final_;
  const DirectedGraphBase* graph_;
  absl::string_view cur_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_GRAPH_WALK_H

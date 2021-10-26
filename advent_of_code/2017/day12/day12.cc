#include "advent_of_code/2017/day12/day12.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/directed_graph.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::StatusOr<DirectedGraph<bool>> Parse(absl::Span<absl::string_view> input) {
  DirectedGraph<bool> ret;
  for (absl::string_view str : input) {
    std::vector<absl::string_view> node_and_cons = absl::StrSplit(str, " <-> ");
    if (node_and_cons.size() != 2) return AdventDay::Error("Bad line: ", str);
    absl::string_view node = node_and_cons[0];
    std::vector<absl::string_view> cons =
        absl::StrSplit(node_and_cons[1], ", ");
    for (absl::string_view con : cons) {
      ret.AddEdge(node, con);
    }
  }
  return ret;
}

class PathWalk : public BFSInterface<PathWalk, absl::string_view> {
 public:
  PathWalk(const DirectedGraph<bool>& graph, absl::string_view start)
      : graph_(graph), cur_(start) {}

  int FindReachable() {
    int reachable = 0;
    reachable_ = &reachable;
    FindMinSteps();
    return reachable;
  }

  int CountGroups() {
    int groups = 0;
    absl::flat_hash_set<absl::string_view> nodes = graph_.nodes();
    to_see_ = &nodes;
    while (!nodes.empty()) {
      ++groups;
      cur_ = *nodes.begin();
      FindMinSteps();
    }
    return groups;
  }

  bool IsFinal() override { return false; }

  void AddNextSteps(State* state) override {
    if (reachable_ != nullptr) ++*reachable_;
    if (to_see_ != nullptr) to_see_->erase(cur_);

    const std::vector<absl::string_view>* outgoing = graph_.Outgoing(cur_);
    if (outgoing != nullptr) {
      for (absl::string_view next_cur : *outgoing) {
        PathWalk next = *this;
        next.cur_ = next_cur;
        state->AddNextStep(next);
      }
    }
  }

  absl::string_view identifier() const override { return cur_; }

 private:
  const DirectedGraph<bool>& graph_;
  absl::string_view cur_;
  int* reachable_ = nullptr;
  absl::flat_hash_set<absl::string_view>* to_see_ = nullptr;
};

}  // namespace

absl::StatusOr<std::string> Day_2017_12::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<bool>> graph = Parse(input);
  if (!graph.ok()) return graph.status();

  return IntReturn(PathWalk(*graph, "0").FindReachable());
}

absl::StatusOr<std::string> Day_2017_12::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<bool>> graph = Parse(input);
  if (!graph.ok()) return graph.status();

  return IntReturn(PathWalk(*graph, "0").CountGroups());
}

}  // namespace advent_of_code

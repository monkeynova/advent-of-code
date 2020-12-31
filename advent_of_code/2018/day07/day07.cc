#include "advent_of_code/2018/day07/day07.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day07_2018::Part1(
    absl::Span<absl::string_view> input) const {
  DirectedGraph<bool> graph;
  for (absl::string_view row : input) {
    absl::string_view src;
    absl::string_view dst;
    if (!RE2::FullMatch(row, "Step (.*) must be finished before step (.*) can begin.", &src, &dst)) {
      return Error("Bad input: ", row);
    }
    graph.AddEdge(src, dst);
  }
  absl::StatusOr<std::vector<absl::string_view>> ordered = graph.DAGSort();
  if (!ordered.ok()) return ordered.status();
  return std::vector<std::string>{absl::StrJoin(*ordered, "")};
}

absl::StatusOr<std::vector<std::string>> Day07_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

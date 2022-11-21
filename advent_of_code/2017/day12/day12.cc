// http://adventofcode.com/2017/day/12

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
    const auto [node, cons] = PairSplit(str, " <-> ");
    for (absl::string_view con : absl::StrSplit(cons, ", ")) {
      ret.AddEdge(node, con);
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_12::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<bool>> graph = Parse(input);
  if (!graph.ok()) return graph.status();

  return IntReturn(graph->Reachable("0").size());
}

absl::StatusOr<std::string> Day_2017_12::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<bool>> graph = Parse(input);
  if (!graph.ok()) return graph.status();

  return IntReturn(graph->Forest().size());
}

}  // namespace advent_of_code

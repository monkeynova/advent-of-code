#include "advent_of_code/2017/day12/day12.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/directed_graph.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::StatusOr<DirectedGraph<bool>> Parse(absl::Span<std::string_view> input) {
  DirectedGraph<bool> ret;
  for (std::string_view str : input) {
    const auto [node, cons] = PairSplit(str, " <-> ");
    for (std::string_view con : absl::StrSplit(cons, ", ")) {
      ret.AddEdge(node, con);
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_12::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(DirectedGraph<bool> graph, Parse(input));

  return AdventReturn(graph.Reachable("0").size());
}

absl::StatusOr<std::string> Day_2017_12::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(DirectedGraph<bool> graph, Parse(input));

  return AdventReturn(graph.Forest().size());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/12, []() {
  return std::unique_ptr<AdventDay>(new Day_2017_12());
});

}  // namespace advent_of_code

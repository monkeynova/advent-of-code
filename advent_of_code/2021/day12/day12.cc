#include "advent_of_code/2021/day12/day12.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct State {
  std::string_view cur;
  absl::flat_hash_set<std::string_view> small_visit;
  bool has_visit2;
};

absl::StatusOr<int64_t> CountPaths(const Graph<int64_t>& map,
                                   bool allow_visit2) {
  constexpr std::string_view kStartNode = "start";
  constexpr std::string_view kEndNode = "end";

  if (!map.nodes().contains(kStartNode)) {
    return absl::NotFoundError("No start");
  }
  if (!map.nodes().contains(kEndNode)) {
    return absl::NotFoundError("No end");
  }

  int64_t path_count = 0;
  State start{.cur = kStartNode, .small_visit = {kStartNode}};
  for (std::deque<State> queue = {start}; !queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    const std::vector<std::string_view>* out = map.Outgoing(cur.cur);
    if (!out) continue;
    for (std::string_view next : *out) {
      if (next == kEndNode) {
        ++path_count;
        continue;
      }
      State new_state = cur;
      new_state.cur = next;
      if (cur.small_visit.contains(next)) {
        if (!allow_visit2) continue;
        if (cur.has_visit2) continue;
        if (next == kStartNode) continue;
        new_state.has_visit2 = true;
      }
      if (next[0] >= 'a' && next[0] <= 'z') {
        new_state.small_visit.insert(next);
      }
      queue.push_back(std::move(new_state));
    }
  }
  return path_count;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_12::Part1(
    absl::Span<std::string_view> input) const {
  Graph<int64_t> map;
  for (std::string_view line : input) {
    auto [src, dst] = PairSplit(line, "-");
    map.AddEdge(src, dst);
  }

  return AdventReturn(CountPaths(map, /*allow_visit2=*/false));
}

absl::StatusOr<std::string> Day_2021_12::Part2(
    absl::Span<std::string_view> input) const {
  Graph<int64_t> map;
  for (std::string_view line : input) {
    auto [src, dst] = PairSplit(line, "-");
    map.AddEdge(src, dst);
  }

  return AdventReturn(CountPaths(map, /*allow_visit2=*/true));
}

}  // namespace advent_of_code

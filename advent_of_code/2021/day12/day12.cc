#include "advent_of_code/2021/day12/day12.h"

#include "absl/algorithm/container.h"
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

struct State {
  absl::string_view cur;
  absl::flat_hash_set<absl::string_view> small_visit;
  bool has_visit2;
};

int64_t CountPaths(const DirectedGraph<int64_t>& map, bool allow_small_visit2) {
  int64_t path_count = 0;
  State start{.cur = "start", .small_visit = {"start"}};
  for (std::deque<State> queue = {start}; !queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    const std::vector<absl::string_view>* out = map.Outgoing(cur.cur);
    if (!out) continue;
    for (absl::string_view next : *out) {
      if (next == "end") {
        ++path_count;
        continue;
      }
      State new_state = cur;
      new_state.cur = next;
      if (cur.small_visit.contains(next)) {
        if (!allow_small_visit2) continue;
        if (cur.has_visit2) continue;
        if (next == "start") continue;
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
    absl::Span<absl::string_view> input) const {
  DirectedGraph<int64_t> map;
  for (absl::string_view line : input) {
    auto [src, dst] = PairSplit(line, "-");
    map.AddEdge(src, dst);
    map.AddEdge(dst, src);
  }

  return IntReturn(CountPaths(map, /*allow_small_visit2=*/false));
}

absl::StatusOr<std::string> Day_2021_12::Part2(
    absl::Span<absl::string_view> input) const {
  DirectedGraph<int64_t> map;
  for (absl::string_view line : input) {
    auto [src, dst] = PairSplit(line, "-");
    map.AddEdge(src, dst);
    map.AddEdge(dst, src);
  }

  return IntReturn(CountPaths(map, /*allow_small_visit2=*/true));
}

}  // namespace advent_of_code

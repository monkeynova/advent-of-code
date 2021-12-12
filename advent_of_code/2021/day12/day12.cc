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
  std::vector<absl::string_view> path;
  absl::flat_hash_set<absl::string_view> small_visit;
  absl::string_view small_visit2;

  const bool operator==(const State& o) const {
    return path == o.path;
  }

  template <typename H>
  friend H AbslHashValue(H h, const State& o) {
    return H::combine(std::move(h), o.path);
  }
};

int64_t CountPaths(const DirectedGraph<int64_t>& map) {
  absl::flat_hash_set<State> hist;
  absl::flat_hash_set<State> end;
  State start{.path = {"start"}, .small_visit = {"start"}};
  for (std::deque<State> queue = {start}; !queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    VLOG(3) << "cur: " << absl::StrJoin(cur.path, "->");
    const std::vector<absl::string_view>* out = map.Outgoing(cur.path.back());
    if (!out) continue;
    for (absl::string_view next : *out) {
      if (cur.small_visit.contains(next)) continue;
      State new_state = cur;
      new_state.path.push_back(next);
      if (next[0] >= 'a' && next[0] <= 'z') {
        VLOG(3) << "Small: " << next;
        new_state.small_visit.insert(next);
      }
      if (next == "end") {
        end.insert(new_state);
        VLOG(3) << "end: " << absl::StrJoin(new_state.path, "->");
        continue;
      }
      if (hist.contains(new_state)) continue;
      hist.insert(new_state);
      queue.push_back(new_state);
    }
  }
  return end.size();
}

int64_t CountPaths2(const DirectedGraph<int64_t>& map) {
  absl::flat_hash_set<State> hist;
  absl::flat_hash_set<State> end;
  State start{.path = {"start"}, .small_visit = {"start"}};
  for (std::deque<State> queue = {start}; !queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    VLOG(3) << "cur: " << absl::StrJoin(cur.path, "->");
    const std::vector<absl::string_view>* out = map.Outgoing(cur.path.back());
    if (!out) continue;
    for (absl::string_view next : *out) {
      State new_state = cur;
      new_state.path.push_back(next);
      if (cur.small_visit.contains(next)) {
        if (!cur.small_visit2.empty()) continue;
        if (next == "start") continue;
        new_state.small_visit2 = next;
      }
      if (next[0] >= 'a' && next[0] <= 'z') {
        VLOG(3) << "Small: " << next;
        new_state.small_visit.insert(next);
      }
      if (next == "end") {
        end.insert(new_state);
        VLOG(3) << "end: " << absl::StrJoin(new_state.path, "->");
        continue;
      }
      if (hist.contains(new_state)) continue;
      hist.insert(new_state);
      queue.push_back(new_state);
    }
  }
  return end.size();
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

  return IntReturn(CountPaths(map));
}

absl::StatusOr<std::string> Day_2021_12::Part2(
    absl::Span<absl::string_view> input) const {
  DirectedGraph<int64_t> map;
  for (absl::string_view line : input) {
    auto [src, dst] = PairSplit(line, "-");
    map.AddEdge(src, dst);
    map.AddEdge(dst, src);
  }

  return IntReturn(CountPaths2(map));
}

}  // namespace advent_of_code

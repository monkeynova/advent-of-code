// http://adventofcode.com/2023/day/17

#include "advent_of_code/2023/day17/day17.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct State {
  Point p;
  Point dir;
  int moved;

  bool operator==(const State& o) const {
    return p == o.p && dir == o.dir && moved == o.moved;
  }
  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.p, s.dir, s.moved);
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2023_17::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  std::deque<State> queue;
  queue.push_back({.p = {0, 0}, .dir = Cardinal::kEast, .moved = 0});
  queue.push_back({.p = {0, 0}, .dir = Cardinal::kSouth, .moved = 0});
  absl::flat_hash_map<State, int> heat;
  for (State s : queue) heat[s] = 0;
  int answer = std::numeric_limits<int>::max();
  for (;!queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    auto it = heat.find(cur);
    CHECK(it != heat.end());
    int cur_heat = it->second;
    for (Point d : {cur.dir, cur.dir.rotate_left(), cur.dir.rotate_right()}) {
      State n = cur;
      n.p += d;
      if (!b.OnBoard(n.p)) continue;
      if (n.dir == d) {
        ++n.moved;
      } else {
        n.moved = 1;
        n.dir = d;
      }
      if (n.moved > 3) continue;
      int next_heat = cur_heat + b[n.p] - '0';
      {
        auto [it, inserted] = heat.emplace(n, next_heat);
        if (inserted || next_heat < it->second) {
          it->second = next_heat;
          queue.push_back(n);
          if (n.p == Point{b.width() - 1, b.height() - 1}) {
            answer = std::min(answer, next_heat);
          }
        }
      }
    }
  }
  return AdventReturn(answer);
}

void AddRange(
    const CharBoard& b,
    State s, Point dir,
    int start, int end,
    absl::flat_hash_map<State, int>& heat_map,
    std::deque<State>& queue) {
  auto it = heat_map.find(s);
  CHECK(it != heat_map.end());
  int heat = it->second;
  CHECK(s.dir != dir);
  s.dir = dir;
  for (int i = 0; i < end; ++i) {
    s.p += s.dir;
    if (!b.OnBoard(s.p)) break;
    heat += b[s.p] - '0';
    if (i + 1 < start) continue;

    auto [it, inserted] = heat_map.emplace(s, heat);
    if (inserted || it->second > heat) {
      it->second = heat;
      queue.push_back(s);
    }
  }
}


absl::StatusOr<std::string> Day_2023_17::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  std::deque<State> queue;
  absl::flat_hash_map<State, int> heat;
  for (Point d : {Cardinal::kEast, Cardinal::kSouth}) {
    State s = {.p = {0, 0}, .dir = Cardinal::kOrigin, .moved = 0};
    heat[s] = 0;
    AddRange(b, s, d, 4, 10, heat, queue);
  }
  for (;!queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    AddRange(b, cur, cur.dir.rotate_left(), 4, 10, heat, queue);
    AddRange(b, cur, cur.dir.rotate_right(), 4, 10, heat, queue);
  }
  int answer = std::numeric_limits<int>::max();
  Point end = {b.width() - 1, b.height() - 1};
  for (Point d : {Cardinal::kEast, Cardinal::kSouth}) {
    auto it = heat.find({.p = end, .dir = d, .moved = 0});
    if (it != heat.end()) {
      answer = std::min(answer, it->second);
    }
  }
  return AdventReturn(answer);
}

}  // namespace advent_of_code

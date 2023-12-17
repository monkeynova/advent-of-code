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

enum Dir {
  kNorth = 0,
  kSouth = 1,
  kWest = 2,
  kEast = 3,
};

constexpr std::array<Dir, 4> kRotateLeft = {kWest, kEast, kSouth, kNorth};
constexpr std::array<Dir, 4> kRotateRight = {kEast, kWest, kNorth, kSouth};

using State = std::pair<int, Dir>;

void AddRange(
    const CharBoard& b,
    State s, Dir dir,
    int start, int end,
    absl::flat_hash_map<State, int>& heat_map,
    std::deque<State>& queue) {
  auto it = heat_map.find(s);
  CHECK(it != heat_map.end());
  int heat = it->second;
  CHECK(s.second != dir);
  s.second = dir;
  const char* base = b.row(0).data();
  int stride = b.row(1).data() - b.row(0).data();
  const std::array<int, 4> kIdxDelta = {-stride, stride, -1, 1};
  int delta = kIdxDelta[dir];
  int max_idx = stride * (b.height() - 1) + b.width() - 1;
  auto on_board = [&](int idx) {
    if (idx < 0) return false;
    if (idx > max_idx) return false;
    return idx % stride != stride - 1;
  };
  for (int i = 0; i < end; ++i) {
    s.first += delta;
    if (!on_board(s.first)) break;
    heat += base[s.first] - '0';
    if (i + 1 < start) continue;

    auto [it, inserted] = heat_map.emplace(s, heat);
    if (inserted || it->second > heat) {
      it->second = heat;
      queue.push_back(s);
    }
  }
}

}  // namespace

absl::StatusOr<std::string> Day_2023_17::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  std::deque<State> queue;
  absl::flat_hash_map<State, int> heat;
  for (Dir d : {kEast, kSouth}) {
    State s = {0, kNorth};
    heat[s] = 0;
    AddRange(b, s, d, 1, 3, heat, queue);
  }
  for (;!queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    AddRange(b, cur, kRotateLeft[cur.second], 1, 3, heat, queue);
    AddRange(b, cur, kRotateRight[cur.second], 1, 3, heat, queue);
  }
  int answer = std::numeric_limits<int>::max();
  int stride = b.row(1).data() - b.row(0).data();
  int end = (b.width() - 1) * stride + b.height() - 1;
  for (Dir d : {kEast, kSouth}) {
    auto it = heat.find({end, d});
    if (it != heat.end()) {
      answer = std::min(answer, it->second);
    }
  }
  return AdventReturn(answer);
}

absl::StatusOr<std::string> Day_2023_17::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  std::deque<State> queue;
  absl::flat_hash_map<State, int> heat;
  for (Dir d : {kEast, kSouth}) {
    State s = {0, kNorth};
    heat[s] = 0;
    AddRange(b, s, d, 4, 10, heat, queue);
  }
  for (;!queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    AddRange(b, cur, kRotateLeft[cur.second], 4, 10, heat, queue);
    AddRange(b, cur, kRotateRight[cur.second], 4, 10, heat, queue);
  }
  int answer = std::numeric_limits<int>::max();
  int stride = b.row(1).data() - b.row(0).data();
  int end = (b.width() - 1) * stride + b.height() - 1;
  for (Dir d : {kEast, kSouth}) {
    auto it = heat.find({end, d});
    if (it != heat.end()) {
      answer = std::min(answer, it->second);
    }
  }
  return AdventReturn(answer);
}

}  // namespace advent_of_code

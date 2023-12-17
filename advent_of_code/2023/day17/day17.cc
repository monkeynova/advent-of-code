// http://adventofcode.com/2023/day/17

#include "advent_of_code/2023/day17/day17.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"

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
    const ImmutableCharBoard& b,
    State s, Dir dir,
    int start, int end,
    std::vector<int>& heat_map,
    std::deque<State>& queue) {
  int heat = heat_map[s.first * 4 + s.second];
  CHECK_NE(heat, std::numeric_limits<int>::max());
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

    if (heat < heat_map[s.first * 4 + s.second]) {
      heat_map[s.first * 4 + s.second] = heat;
      queue.push_back(s); 
    }
  }
}

}  // namespace

absl::StatusOr<std::string> Day_2023_17::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  std::deque<State> queue;
  int stride = b.row(1).data() - b.row(0).data();
  int max_idx = stride * (b.height() - 1) + b.width() - 1;
  std::vector<int> heat_map(4 * (max_idx + 1), std::numeric_limits<int>::max());
  for (Dir d : {kEast, kSouth}) {
    State s = {0, kNorth};
    heat_map[kNorth] = 0;
    AddRange(b, s, d, 1, 3, heat_map, queue);
  }
  for (;!queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    AddRange(b, cur, kRotateLeft[cur.second], 1, 3, heat_map, queue);
    AddRange(b, cur, kRotateRight[cur.second], 1, 3, heat_map, queue);
  }
  int answer = std::numeric_limits<int>::max();
  answer = std::min(answer, heat_map[max_idx * 4 + kEast]);
  answer = std::min(answer, heat_map[max_idx * 4 + kSouth]);
  return AdventReturn(answer);
}

absl::StatusOr<std::string> Day_2023_17::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  std::deque<State> queue;
  int stride = b.row(1).data() - b.row(0).data();
  int max_idx = stride * (b.height() - 1) + b.width() - 1;
  std::vector<int> heat_map(4 * (max_idx + 1), std::numeric_limits<int>::max());
  for (Dir d : {kEast, kSouth}) {
    State s = {0, kNorth};
    heat_map[kNorth] = 0;
    AddRange(b, s, d, 4, 10, heat_map, queue);
  }
  for (;!queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    AddRange(b, cur, kRotateLeft[cur.second], 4, 10, heat_map, queue);
    AddRange(b, cur, kRotateRight[cur.second], 4, 10, heat_map, queue);
  }
  int answer = std::numeric_limits<int>::max();
  answer = std::min(answer, heat_map[max_idx * 4 + kEast]);
  answer = std::min(answer, heat_map[max_idx * 4 + kSouth]);
  return AdventReturn(answer);
}

}  // namespace advent_of_code

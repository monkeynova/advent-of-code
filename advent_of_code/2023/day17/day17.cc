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

int MinCartPath(const ImmutableCharBoard& b, int min, int max) {
  std::deque<State> queue;
  int stride = b.row(1).data() - b.row(0).data();
  int max_idx = stride * (b.height() - 1) + b.width() - 1;
  std::vector<int> heat_map(4 * (max_idx + 1), std::numeric_limits<int>::max());
  std::vector<bool> in_queue(4 * (max_idx + 1), false);

  std::vector<bool> on_board_vec(max_idx + 1, true);
  for (int i = stride - 1; i <= max_idx; i += stride) {
    on_board_vec[i] = false;
  }

  auto on_board = [&](int idx) -> bool {
    if (idx < 0) return false;
    if (idx > max_idx) return false;
    return on_board_vec[idx];
  };

  const std::array<int, 4> kIdxDelta = {-stride, stride, -1, 1};

  auto add_range = [&](State s, Dir dir) {
    int heat = heat_map[s.first * 4 + s.second];
    CHECK_NE(heat, std::numeric_limits<int>::max());
    CHECK(s.second != dir);
    s.second = dir;
    const char* base = b.row(0).data();
    int delta = kIdxDelta[dir];
    for (int i = 0; i < max; ++i) {
      s.first += delta;
      if (!on_board(s.first)) break;
      heat += base[s.first] - '0';
      if (i + 1 < min) continue;
  
      if (heat < heat_map[s.first * 4 + s.second]) {
        heat_map[s.first * 4 + s.second] = heat;
        if (!in_queue[s.first * 4 + s.second]) {
          in_queue[s.first * 4 + s.second] = true;
          queue.push_back(s); 
        }
      }
    }
  };

  for (Dir d : {kEast, kSouth}) {
    State s = {0, kNorth};
    heat_map[kNorth] = 0;
    add_range(s, d);
  }
  for (;!queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    in_queue[cur.first * 4 + cur.second] = false;
    add_range(cur, kRotateLeft[cur.second]);
    add_range(cur, kRotateRight[cur.second]);
  }
  int answer = std::numeric_limits<int>::max();
  answer = std::min(answer, heat_map[max_idx * 4 + kEast]);
  answer = std::min(answer, heat_map[max_idx * 4 + kSouth]);
  return answer;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_17::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  return AdventReturn(MinCartPath(b, 1, 3));
}

absl::StatusOr<std::string> Day_2023_17::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  return AdventReturn(MinCartPath(b, 4, 10));
}

}  // namespace advent_of_code

#include "advent_of_code/2016/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct PathState {
  Point cur;
  int have_keys;
  int num_steps;
  bool operator==(const PathState& o) const {
    return cur == o.cur && have_keys == o.have_keys;
  }
};

template <typename H>
H AbslHashValue(H h, const PathState& s) {
  return H::combine(std::move(h), s.cur, s.have_keys);
}

absl::StatusOr<int> MinPathAllNums(const CharBoard& b) {
  Point start;
  int need_keys = 0;
  for (Point p : b.range()) {
    if (b[p] == '0')
      start = p;
    else if (b[p] > '0' && b[p] <= '9') {
      need_keys |= 1 << (b[p] - '1');
    }
  }
  std::deque<PathState> frontier = {
      PathState{.cur = start, .have_keys = 0, .num_steps = 0}};
  absl::flat_hash_set<PathState> hist;
  hist.insert(frontier.front());
  while (!frontier.empty()) {
    const PathState& cur_state = frontier.front();
    for (Point dir : Cardinal::kFourDirs) {
      Point next_point = cur_state.cur + dir;
      if (b.OnBoard(next_point) && b[next_point] != '#') {
        PathState next_state = cur_state;
        ++next_state.num_steps;
        next_state.cur = next_point;
        if (b[next_point] > '0' && b[next_point] <= '9') {
          next_state.have_keys |= 1 << (b[next_point] - '1');
          if (next_state.have_keys == need_keys) {
            return next_state.num_steps;
          }
        }
        if (!hist.contains(next_state)) {
          hist.insert(next_state);
          frontier.push_back(next_state);
        }
      }
    }

    frontier.pop_front();
  }

  return AdventDay::Error("No path found");
}

absl::StatusOr<int> MinPathAllNumsReturn(const CharBoard& b) {
  Point start;
  int need_keys = 0;
  for (Point p : b.range()) {
    if (b[p] == '0')
      start = p;
    else if (b[p] > '0' && b[p] <= '9') {
      need_keys |= 1 << (b[p] - '1');
    }
  }
  std::deque<PathState> frontier = {
      PathState{.cur = start, .have_keys = 0, .num_steps = 0}};
  absl::flat_hash_set<PathState> hist;
  hist.insert(frontier.front());
  while (!frontier.empty()) {
    const PathState& cur_state = frontier.front();
    for (Point dir : Cardinal::kFourDirs) {
      Point next_point = cur_state.cur + dir;
      if (b.OnBoard(next_point) && b[next_point] != '#') {
        PathState next_state = cur_state;
        ++next_state.num_steps;
        next_state.cur = next_point;
        if (b[next_point] > '0' && b[next_point] <= '9') {
          next_state.have_keys |= 1 << (b[next_point] - '1');
        }
        if (next_state.have_keys == need_keys && next_state.cur == start) {
          return next_state.num_steps;
        }
        if (!hist.contains(next_state)) {
          hist.insert(next_state);
          frontier.push_back(next_state);
        }
      }
    }

    frontier.pop_front();
  }

  return AdventDay::Error("No path found");
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day24_2016::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  return IntReturn(MinPathAllNums(*b));
}

absl::StatusOr<std::vector<std::string>> Day24_2016::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  return IntReturn(MinPathAllNumsReturn(*b));
}

}  // namespace advent_of_code

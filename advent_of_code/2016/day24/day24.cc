#include "advent_of_code/2016/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point_walk.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2016_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  absl::flat_hash_set<Point> start_set = b->Find('0');
  if (start_set.size() != 1) return Error("No unique start");
  Point start = *start_set.begin();

  int need = 0;
  for (const auto [_, c] : *b) {
    if (c > '0' && c <= '9') {
      need |= 1 << (c - '1');
    }
  }

  return AdventReturn(PointWalkData<int>({
    .start = {.p = start, .d = need},
    .is_good = [&](PointAndData<int>& cur, int) {
      if (!b->OnBoard(cur.p)) return false;
      char board_char = (*b)[cur.p];
      if (board_char == '#') return false;
      if (board_char > '0' && board_char <= '9') {
        cur.d &= ~(1 << (board_char - '1'));
      }
      return true;
    },
    .is_final = [&](PointAndData<int> cur, int) {
      return cur.d == 0;
    },
  }).FindMinSteps());
}

absl::StatusOr<std::string> Day_2016_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  absl::flat_hash_set<Point> start_set = b->Find('0');
  if (start_set.size() != 1) return Error("No unique start");
  Point start = *start_set.begin();

  int need = 0;
  for (const auto [_, c] : *b) {
    if (c > '0' && c <= '9') {
      need |= 1 << (c - '1');
    }
  }

  return AdventReturn(PointWalkData<int>({
    .start = {.p = start, .d = need},
    .is_good = [&](PointAndData<int>& cur, int) {
      if (!b->OnBoard(cur.p)) return false;
      char board_char = (*b)[cur.p];
      if (board_char == '#') return false;
      if (board_char > '0' && board_char <= '9') {
        cur.d &= ~(1 << (board_char - '1'));
      }
      return true;
    },
    .is_final = [&](PointAndData<int> cur, int) {
      return cur.d == 0 && cur.p == start;
    },
  }).FindMinSteps());
}

}  // namespace advent_of_code

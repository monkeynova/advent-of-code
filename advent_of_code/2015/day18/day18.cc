#include "advent_of_code/2015/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/conway.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_18::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input);
  if (!board.ok()) return board.status();

  Conway conway(*board);
  if (auto st = conway.AdvanceN(100); !st.ok()) return st;

  return AdventReturn(conway.board().CountOn());
}

absl::StatusOr<std::string> Day_2015_18::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input);
  if (!board.ok()) return board.status();

  Conway conway(*board);
  const std::array<Point, 4> kCorners = {
      board->range().min,
      board->range().max,
      {board->range().min.x, board->range().max.y},
      {board->range().max.x, board->range().min.y},
  };
  for (int i = 0; i < 100; ++i) {
    if (auto st = conway.Advance(); !st.ok()) return st;
    for (Point p : kCorners) {
      conway.board()[p] = '#';
    }
  }
  return AdventReturn(conway.board().CountOn());
}

}  // namespace advent_of_code

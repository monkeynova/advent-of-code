#include "advent_of_code/2022/day08/day08.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2022_08::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard board, CharBoard::Parse(input));

  int count = 0;
  for (Point p : board.range()) {
    bool visible_any = false;
    for (Point d : Cardinal::kFourDirs) {
      bool visible_this = true;
      for (Point t = p + d; board.OnBoard(t); t += d) {
        if (board[t] >= board[p]) {
          visible_this = false;
          break;
        }
      }
      if (visible_this) {
        visible_any = true;
        break;
      }
    }
    if (visible_any) ++count;
  }
  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2022_08::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard board, CharBoard::Parse(input));

  int best_score = 0;
  for (Point p : board.range()) {
    int score = 1;
    for (Point d : Cardinal::kFourDirs) {
      int dist = 0;
      for (Point t = p + d; board.OnBoard(t); t += d) {
        ++dist;
        if (board[t] >= board[p]) {
          break;
        }
      }
      score *= dist;
    }
    best_score = std::max(best_score, score);
  }
  return AdventReturn(best_score);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/8, []() {
  return std::unique_ptr<AdventDay>(new Day_2022_08());
});

}  // namespace advent_of_code

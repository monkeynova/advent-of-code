#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2022_08::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();

  int count = 0;
  for (Point p : board->range()) {
    bool visible_any = false;
    for (Point d : Cardinal::kFourDirs) {
      bool visible_this = true;
      for (Point t = p + d; board->OnBoard(t); t += d) {
        if ((*board)[t] >= (*board)[p]) {
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
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2022_08::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();

  int best_score = 0;
  for (Point p : board->range()) {
    int score = 1;
    for (Point d : Cardinal::kFourDirs) {
      int dist = 0;
      for (Point t = p + d; board->OnBoard(t); t += d) {
        ++dist;
        if ((*board)[t] >= (*board)[p]) {
          break;
        }
      }
      score *= dist;
    }
    best_score = std::max(best_score, score);
  }
  return IntReturn(best_score);
}

}  // namespace advent_of_code

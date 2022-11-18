#include "advent_of_code/2021/day25/day25.h"

#include "absl/algorithm/container.h"
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

int Update(CharBoard& b, char fish, Point dir) {
  absl::flat_hash_map<Point, Point> move;
  for (Point p : b.range()) {
    if (b[p] == fish) {
      Point n = b.TorusPoint(p + dir);
      if (b[n] == '.') move[p] = n;
    }
  }
  for (const auto& [p, n] : move) {
    std::swap(b[p], b[n]);
  }
  return move.size();
}

int UpdateEast(CharBoard& b) { return Update(b, '>', Cardinal::kEast); }

int UpdateSouth(CharBoard& b) { return Update(b, 'v', Cardinal::kSouth); }

}  // namespace

absl::StatusOr<std::string> Day_2021_25::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  for (int i = 1; true; ++i) {
    int moved = 0;
    moved += UpdateEast(*b);
    moved += UpdateSouth(*b);
    if (moved == 0) {
      return IntReturn(i);
    }
  }

  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2021_25::Part2(
    absl::Span<absl::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
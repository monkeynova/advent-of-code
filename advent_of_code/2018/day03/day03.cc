#include "advent_of_code/2018/day03/day03.h"

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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_03::Part1(
    absl::Span<absl::string_view> input) const {
  // #1372 @ 119,13: 29x16
  CharBoard board(1'000, 1'000);
  for (Point p : board.range()) board[p] = '.';
  int dupes = 0;
  for (absl::string_view row : input) {
    PointRectangle r;
    if (!RE2::FullMatch(row, "#\\d+ @ (\\d+),(\\d+): (\\d+)x(\\d+)", &r.min.x,
                        &r.min.y, &r.max.x, &r.max.y)) {
      return Error("Bad input: ", row);
    }
    r.max.x += r.min.x - 1;
    r.max.y += r.min.y - 1;
    if (!board.OnBoard(r.min)) return Error("Min off board");
    if (!board.OnBoard(r.max)) return Error("Max off board");
    for (Point p : r) {
      if (board[p] == '+') {
        ++dupes;
        board[p] = '#';
      } else if (board[p] == '#') {
        // Already counted.
      } else {
        board[p] = '+';
      }
    }
  }
  return IntReturn(dupes);
}

absl::StatusOr<std::string> Day_2018_03::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<PointRectangle> claims;
  for (int i = 0; i < input.size(); ++i) {
    absl::string_view row = input[i];
    PointRectangle r;
    int idx;
    if (!RE2::FullMatch(row, "#(\\d+) @ (\\d+),(\\d+): (\\d+)x(\\d+)", &idx,
                        &r.min.x, &r.min.y, &r.max.x, &r.max.y)) {
      return Error("Bad input: ", row);
    }
    if (idx != i + 1) return Error("Bad numbering");
    r.max.x += r.min.x - 1;
    r.max.y += r.min.y - 1;
    claims.push_back(r);
  }

  int clean_id = -1;
  for (int i = 0; i < claims.size(); ++i) {
    const PointRectangle& r1 = claims[i];
    bool overlap = false;
    for (int j = 0; j < claims.size(); ++j) {
      if (i == j) continue;
      const PointRectangle& r2 = claims[j];
      PointRectangle intersect;
      intersect.min.x = std::max(r1.min.x, r2.min.x);
      intersect.min.y = std::max(r1.min.y, r2.min.y);
      intersect.max.x = std::min(r1.max.x, r2.max.x);
      intersect.max.y = std::min(r1.max.y, r2.max.y);
      if (intersect.min.x <= intersect.max.x &&
          intersect.min.y <= intersect.max.y) {
        overlap = true;
        break;
      }
    }
    if (!overlap) {
      if (clean_id != -1) return Error("Dupe clean");
      clean_id = i + 1;
    }
  }
  return IntReturn(clean_id);
}

}  // namespace advent_of_code

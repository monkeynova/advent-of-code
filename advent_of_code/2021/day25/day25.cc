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

absl::StatusOr<CharBoard> UpdateEast(const CharBoard& b) {
  CharBoard out(b.width(), b.height());
  absl::flat_hash_set<Point> skip;
  for (Point p : b.range()) {
    if (skip.contains(p)) continue;
    if (b[p] == '>') {
      Point n = p;
      n.x = (n.x + 1) % b.width();
      if (b[n] == '.') {
        out[n] = '>';
        out[p] = '.';
        skip.insert(n);
      } else {
        out[p] = b[p];
      }
    } else {
      out[p] = b[p];
    }
  }
  return out;
}
absl::StatusOr<CharBoard> UpdateSouth(const CharBoard& b) {
  CharBoard out(b.width(), b.height());
  absl::flat_hash_set<Point> skip;
  for (Point p : b.range()) {
    if (skip.contains(p)) continue;
    if (b[p] == 'v') {
      Point n = p;
      n.y = (n.y + 1) % b.height();
      if (b[n] == '.') {
        out[n] = 'v';
        out[p] = '.';
        skip.insert(n);
      } else {
        out[p] = b[p];
      }
    } else {
      out[p] = b[p];
    }
  }
  return out;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_25::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  for (int i = 1; true; ++i) {
    absl::StatusOr<CharBoard> new_b = UpdateEast(*b);
    if (!new_b.ok()) return new_b.status();
    new_b = UpdateSouth(*new_b);
    if (!new_b.ok()) return new_b.status();
    if (*new_b == *b) {
      return IntReturn(i);
    }
    b = std::move(*new_b);
  }

  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2021_25::Part2(
    absl::Span<absl::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code

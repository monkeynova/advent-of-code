#include "advent_of_code/2016/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2016_18::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  CharBoard b(input[0].size(), 40);
  absl::flat_hash_set<int> next_traps = {6, 3, 4, 1};
  for (Point p : b.range()) {
    if (p.y == 0) {
      b[p] = input[0][p.x];
    } else {
      int prev_traps = 0;
      for (Point dir :
           {Cardinal::kNorthWest, Cardinal::kNorth, Cardinal::kNorthEast}) {
        prev_traps <<= 1;
        Point prev = p + dir;
        if (b.OnBoard(prev) && b[prev] == '^') {
          prev_traps |= 1;
        }
      }
      b[p] = next_traps.contains(prev_traps) ? '^' : '.';
    }
  }
  LOG(INFO) << "Board:\n" << b;
  return AdventReturn(b.CountChar('.'));
}

absl::StatusOr<std::string> Day_2016_18::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  CharBoard b(input[0].size(), 400000);
  absl::flat_hash_set<int> next_traps = {6, 3, 4, 1};
  for (Point p : b.range()) {
    if (p.y == 0) {
      b[p] = input[0][p.x];
    } else {
      int prev_traps = 0;
      for (Point dir :
           {Cardinal::kNorthWest, Cardinal::kNorth, Cardinal::kNorthEast}) {
        prev_traps <<= 1;
        Point prev = p + dir;
        if (b.OnBoard(prev) && b[prev] == '^') {
          prev_traps |= 1;
        }
      }
      b[p] = next_traps.contains(prev_traps) ? '^' : '.';
    }
  }
  VLOG(2) << "Board:\n" << b;
  return AdventReturn(b.CountChar('.'));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/18,
    []() { return std::unique_ptr<AdventDay>(new Day_2016_18()); });

}  // namespace advent_of_code

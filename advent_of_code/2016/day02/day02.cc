#include "advent_of_code/2016/day02/day02.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_02::Part1(
    absl::Span<absl::string_view> input) const {
  std::string code;
  Point p = {1, 1};
  absl::flat_hash_map<Point, std::string> codes = {
      {{0, 0}, "1"}, {{1, 0}, "2"}, {{2, 0}, "3"}, {{0, 1}, "4"}, {{1, 1}, "5"},
      {{2, 1}, "6"}, {{0, 2}, "7"}, {{1, 2}, "8"}, {{2, 2}, "9"},
  };
  for (absl::string_view ins : input) {
    for (char c : ins) {
      Point dir;
      switch (c) {
        case 'U':
          dir = Cardinal::kNorth;
          break;
        case 'D':
          dir = Cardinal::kSouth;
          break;
        case 'L':
          dir = Cardinal::kWest;
          break;
        case 'R':
          dir = Cardinal::kEast;
          break;
        default:
          return Error("Bad instruction: ", ins);
      }
      Point next = p + dir;
      if (codes.contains(next)) {
        p = next;
      }
    }
    auto it = codes.find(p);
    if (it == codes.end()) return Error("Bad point: ", p);
    code += it->second;
  }
  return code;
}

absl::StatusOr<std::string> Day_2016_02::Part2(
    absl::Span<absl::string_view> input) const {
  std::string code;
  Point p = {0, 2};
  absl::flat_hash_map<Point, std::string> codes = {
      {{2, 0}, "1"}, {{1, 1}, "2"}, {{2, 1}, "3"}, {{3, 1}, "4"}, {{0, 2}, "5"},
      {{1, 2}, "6"}, {{2, 2}, "7"}, {{3, 2}, "8"}, {{4, 2}, "9"}, {{1, 3}, "A"},
      {{2, 3}, "B"}, {{3, 3}, "C"}, {{2, 4}, "D"},
  };
  for (absl::string_view ins : input) {
    for (char c : ins) {
      Point dir;
      switch (c) {
        case 'U':
          dir = Cardinal::kNorth;
          break;
        case 'D':
          dir = Cardinal::kSouth;
          break;
        case 'L':
          dir = Cardinal::kWest;
          break;
        case 'R':
          dir = Cardinal::kEast;
          break;
        default:
          return Error("Bad instruction: ", ins);
      }
      Point next = p + dir;
      if (codes.contains(next)) {
        p = next;
      }
    }
    auto it = codes.find(p);
    if (it == codes.end()) return Error("Bad point: ", p);
    code += it->second;
  }
  return code;
}

}  // namespace advent_of_code

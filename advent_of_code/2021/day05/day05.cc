#include "advent_of_code/2021/day05/day05.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_05::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<Point, int64_t> counts;
  for (absl::string_view line : input) {
    Point p1;
    Point p2;
    if (!RE2::FullMatch(line, "(\\d+),(\\d+) -> (\\d+),(\\d+)", &p1.x, &p1.y, &p2.x, &p2.y)) {
      return Error("Bad line: ", line);
    }
    if (p1.x == p2.x) {
      if (p1.y > p2.y) {
        std::swap(p1, p2);
      }
      for (int y = p1.y; y <= p2.y; ++y) {
        ++counts[{p1.x, y}];
      }
    }
    if (p1.y == p2.y) {
      if (p1.x > p2.x) {
        std::swap(p1, p2);
      }
      for (int x = p1.x; x <= p2.x; ++x) {
        ++counts[{x, p1.y}];
      }
    }
  }
  int64_t ret = 0;
  for (const auto& [_, count] : counts) {
    if (count > 1) ++ret;
  }
  return IntReturn(ret);
}

absl::StatusOr<std::string> Day_2021_05::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<Point, int64_t> counts;
  for (absl::string_view line : input) {
    Point p1;
    Point p2;
    if (!RE2::FullMatch(line, "(\\d+),(\\d+) -> (\\d+),(\\d+)", &p1.x, &p1.y, &p2.x, &p2.y)) {
      return Error("Bad line: ", line);
    }
    Point dir = p2 - p1;
    dir.x = dir.x > 0 ? 1 : dir.x < 0 ? -1 : 0;
    dir.y = dir.y > 0 ? 1 : dir.y < 0 ? -1 : 0;
    for (Point p = p1; p != p2; p += dir) {
      ++counts[p];
    }
    ++counts[p2];
  }
  int64_t ret = 0;
  for (const auto& [_, count] : counts) {
    if (count > 1) ++ret;
  }
  return IntReturn(ret);
}

}  // namespace advent_of_code

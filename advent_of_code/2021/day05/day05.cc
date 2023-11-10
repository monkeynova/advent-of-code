#include "advent_of_code/2021/day05/day05.h"

#include "absl/algorithm/container.h"
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

absl::StatusOr<std::string> Day_2021_05::Part1(
    absl::Span<std::string_view> input) const {
  RE2 line_re("(\\d+,\\d+) -> (\\d+,\\d+)");
  absl::flat_hash_map<Point, int64_t> counts;
  for (std::string_view line : input) {
    Point p1;
    Point p2;
    if (!RE2::FullMatch(line, line_re, p1.Capture(), p2.Capture())) {
      return Error("Bad line: ", line);
    }
    Point dir = (p2 - p1).min_step();
    if (dir.dist() > 1) continue;
    for (Point p = p1; p != p2; p += dir) {
      ++counts[p];
    }
    ++counts[p2];
  }
  int64_t ret = 0;
  for (const auto& [_, count] : counts) {
    if (count > 1) ++ret;
  }
  return AdventReturn(ret);
}

absl::StatusOr<std::string> Day_2021_05::Part2(
    absl::Span<std::string_view> input) const {
  RE2 line_re("(\\d+,\\d+) -> (\\d+,\\d+)");
  absl::flat_hash_map<Point, int64_t> counts;
  for (std::string_view line : input) {
    Point p1;
    Point p2;
    if (!RE2::FullMatch(line, line_re, p1.Capture(), p2.Capture())) {
      return Error("Bad line: ", line);
    }
    Point dir = (p2 - p1).min_step();
    if (dir.dist() > 2) {
      return Error("Found non-0/45/90 degree slope: ", dir);
    }
    for (Point p = p1; p != p2; p += dir) {
      ++counts[p];
    }
    ++counts[p2];
  }
  int64_t ret = 0;
  for (const auto& [_, count] : counts) {
    if (count > 1) ++ret;
  }
  return AdventReturn(ret);
}

}  // namespace advent_of_code

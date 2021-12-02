#include "advent_of_code/2021/day02/day02.h"

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

absl::StatusOr<std::string> Day_2021_02::Part1(
    absl::Span<absl::string_view> input) const {
  Point p{0, 0};
  for (absl::string_view line : input) {
    int64_t delta = 0;
    if (RE2::FullMatch(line, "forward (\\d+)", &delta)) {
      p.x += delta;
    } else if (RE2::FullMatch(line, "up (\\d+)", &delta)) {
      p.y -= delta;
    } else if (RE2::FullMatch(line, "down (\\d+)", &delta)) {
      p.y += delta;
    } else {
      return Error("Bad line: ", line);
    }
  }
  return IntReturn(p.x * p.y);
}

absl::StatusOr<std::string> Day_2021_02::Part2(
    absl::Span<absl::string_view> input) const {
  Point3 p{0, 0, 0};
  for (absl::string_view line : input) {
    int64_t delta = 0;
    if (RE2::FullMatch(line, "forward (\\d+)", &delta)) {
      p.x += delta;
      p.y += p.z * delta;
    } else if (RE2::FullMatch(line, "up (\\d+)", &delta)) {
      p.z -= delta;
    } else if (RE2::FullMatch(line, "down (\\d+)", &delta)) {
      p.z += delta;
    } else {
      return Error("Bad line: ", line);
    }
  }
  return IntReturn(p.x * p.y);
}

}  // namespace advent_of_code

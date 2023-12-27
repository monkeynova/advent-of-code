#include "advent_of_code/2021/day02/day02.h"

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

absl::StatusOr<std::string> Day_2021_02::Part1(
    absl::Span<std::string_view> input) const {
  static const RE2 kForwardRE("forward (\\d+)");
  static const RE2 kUpRE("up (\\d+)");
  static const RE2 kDownRE("down (\\d+)");
  Point p{0, 0};
  for (std::string_view line : input) {
    int64_t delta = 0;
    if (RE2::FullMatch(line, kForwardRE, &delta)) {
      p.x += delta;
    } else if (RE2::FullMatch(line, kUpRE, &delta)) {
      p.y -= delta;
    } else if (RE2::FullMatch(line, kDownRE, &delta)) {
      p.y += delta;
    } else {
      return Error("Bad line: ", line);
    }
  }
  return AdventReturn(p.x * p.y);
}

absl::StatusOr<std::string> Day_2021_02::Part2(
    absl::Span<std::string_view> input) const {
  static const RE2 kForwardRE("forward (\\d+)");
  static const RE2 kUpRE("up (\\d+)");
  static const RE2 kDownRE("down (\\d+)");
  Point p{0, 0};
  int64_t aim = 0;
  for (std::string_view line : input) {
    int64_t delta = 0;
    if (RE2::FullMatch(line, kForwardRE, &delta)) {
      p.x += delta;
      p.y += aim * delta;
    } else if (RE2::FullMatch(line, kUpRE, &delta)) {
      aim -= delta;
    } else if (RE2::FullMatch(line, kDownRE, &delta)) {
      aim += delta;
    } else {
      return Error("Bad line: ", line);
    }
  }
  return AdventReturn(p.x * p.y);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2021, /*day=*/2, []() {
  return std::unique_ptr<AdventDay>(new Day_2021_02());
});

}  // namespace advent_of_code

#include "advent_of_code/2016/day03/day03.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

bool IsValid(int t1[3]) {
  return t1[0] + t1[1] > t1[2] && t1[0] + t1[2] > t1[1] &&
         t1[1] + t1[2] > t1[0];
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_03::Part1(
    absl::Span<std::string_view> input) const {
  int count = 0;
  for (std::string_view in : input) {
    int t1[3];
    if (!RE2::FullMatch(in, "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &t1[0], &t1[1],
                        &t1[2])) {
      return Error("Bad input: ", in);
    }
    if (IsValid(t1)) ++count;
  }
  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2016_03::Part2(
    absl::Span<std::string_view> input) const {
  int count = 0;
  if (input.size() % 3 != 0) return Error("Bad size");
  for (int i = 0; i < input.size(); i += 3) {
    int t1[3];
    int t2[3];
    int t3[3];
    if (!RE2::FullMatch(input[i], "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &t1[0],
                        &t2[0], &t3[0])) {
      return Error("Bad input: ", input[i]);
    }
    if (!RE2::FullMatch(input[i + 1], "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &t1[1],
                        &t2[1], &t3[1])) {
      return Error("Bad input: ", input[i + 1]);
    }
    if (!RE2::FullMatch(input[i + 2], "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &t1[2],
                        &t2[2], &t3[2])) {
      return Error("Bad input: ", input[i + 2]);
    }
    if (IsValid(t1)) ++count;
    if (IsValid(t2)) ++count;
    if (IsValid(t3)) ++count;
  }
  return AdventReturn(count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/3,
    []() { return std::unique_ptr<AdventDay>(new Day_2016_03()); });

}  // namespace advent_of_code

// http://adventofcode.com/2022/day/05

#include "advent_of_code/2022/day05/day05.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_05::Part1(
    absl::Span<absl::string_view> input) const {
  for (absl::string_view line : input) {
    VLOG(1) << line;
  }
  return absl::UnimplementedError("Problem not known");
}

absl::StatusOr<std::string> Day_2022_05::Part2(
    absl::Span<absl::string_view> input) const {
  for (absl::string_view line : input) {
    VLOG(1) << line;
  }
  return absl::UnimplementedError("Problem not known");
}

}  // namespace advent_of_code

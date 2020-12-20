#include "advent_of_code/2016/day03/day03.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day03_2016::Part1(
    absl::Span<absl::string_view> input) const {
  int count = 0;
  for (absl::string_view in : input) {
    int s1;
    int s2;
    int s3;
    if (!RE2::FullMatch(in, "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &s1, &s2, &s3)) {
      return Error("Bad input: ", in);
    }
    bool valid = s1 + s2 > s3 && s1 + s3 > s2 && s2 + s3 > s1;
    if (valid) {
      ++count;
    }
  }
  return IntReturn(count);
}

absl::StatusOr<std::vector<std::string>> Day03_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

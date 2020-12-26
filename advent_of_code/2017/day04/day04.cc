#include "advent_of_code/2017/day04/day04.h"

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

absl::StatusOr<std::vector<std::string>> Day04_2017::Part1(
    absl::Span<absl::string_view> input) const {
  int valid = 0;
  for (absl::string_view in : input) {
    std::vector<absl::string_view> list = absl::StrSplit(in, " ");
    absl::flat_hash_set<absl::string_view> uniq(list.begin(), list.end());
    if (list.size() == uniq.size()) ++valid;
  }
  return IntReturn(valid);
}

absl::StatusOr<std::vector<std::string>> Day04_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

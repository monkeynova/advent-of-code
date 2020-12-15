#include "advent_of_code/2015/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day17_2015::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> cap = ParseAsInts(input);
  if (!cap.ok()) return cap.status();

  int valid = 0;
  for (int i = 0; i < (1 << cap->size()); ++i) {
    int sum = 0;
    for (int bit = 0; (1 << bit) <= i; ++bit) {
      if (i & (1 << bit)) {
        sum += (*cap)[bit];
      }
    }
    if (sum == 150) ++valid;
  }

  return IntReturn(valid);
}

absl::StatusOr<std::vector<std::string>> Day17_2015::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

#include "advent_of_code/2021/day01/day01.h"

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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_01::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();

  int64_t count = 0;
  for (int i = 0; i < list->size() - 1; ++i) {
    if ((*list)[i] < (*list)[i + 1]) ++count;
  }

  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2021_01::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();

  int64_t count = 0;
  for (int i = 0; i < list->size() - 3; ++i) {
    if ((*list)[i] < (*list)[i + 3]) ++count;
  }

  return IntReturn(count);
}

}  // namespace advent_of_code

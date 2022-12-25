#include "advent_of_code/2020/day01/day01.h"

#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_01::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();

  int res = -1;
  absl::flat_hash_set<int> hist;
  for (int a : *list) {
    if (hist.contains(2020 - a)) {
      res = a * (2020 - a);
      break;
    }
    hist.insert(a);
  }
  if (res == -1) return absl::InvalidArgumentError("nothing found");
  return AdventReturn(res);
}

absl::StatusOr<std::string> Day_2020_01::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();

  int res = -1;
  absl::flat_hash_set<int> hist;
  for (int i = 0; i < list->size(); ++i) {
    int a = (*list)[i];
    for (int j = 0; j < list->size(); ++j) {
      int b = (*list)[j];
      if (hist.contains(2020 - a - b)) {
        res = a * b * (2020 - a - b);
        break;
      }
    }
    hist.insert(a);
  }
  if (res == -1) return absl::InvalidArgumentError("nothing found");
  return AdventReturn(res);
}

}  // namespace advent_of_code

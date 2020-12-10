#include "advent_of_code/2020/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"


absl::StatusOr<std::vector<std::string>> Day10_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int64_t>> jolts = ParseAsInts(input);
  if (!jolts.ok()) return jolts.status();
  std::sort(jolts->begin(), jolts->end());
  int cur_j = 0;
  absl::flat_hash_map<int, int> deltas;
  for (int j : *jolts) {
    if (j - cur_j > 3) return absl::InvalidArgumentError("Can't chain");
    ++deltas[j - cur_j];
    cur_j = j;
  }
  // Final adapter.
  ++deltas[3];
  return IntReturn(deltas[1] * deltas[3]);
}

absl::StatusOr<std::vector<std::string>> Day10_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  return IntReturn(-1);
}

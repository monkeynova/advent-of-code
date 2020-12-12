#include "advent_of_code/2020/day09/day09.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<int> FindMissingXMASPair(const std::vector<int64_t>& vals) {
  absl::flat_hash_set<int64_t> hist;
  if (vals.size() < 26) return absl::InvalidArgumentError("Too short");
  for (int i = 0; i < 25; ++i) {
    hist.insert(vals[i]);
  }
  for (int i = 25; i < vals.size(); ++i) {
    bool found = false;
    for (int64_t v : hist) {
      if (vals[i] - v != v && hist.contains(vals[i] - v)) {
        found = true;
        break;
      }
    }
    if (!found) {
      return vals[i];
    }
    hist.insert(vals[i]);
    hist.erase(vals[i - 25]);
  }
  return absl::InvalidArgumentError("Not found");
}

absl::StatusOr<int> FindContiguousRangeMinMaxSum(
    const std::vector<int64_t>& vals, int64_t search_sum) {
  for (int i = 0; i < vals.size(); ++i) {
    for (int j = 0; j < i; ++j) {
      int64_t running_sum = 0;
      int64_t min = std::numeric_limits<int64_t>::max();
      int64_t max = std::numeric_limits<int64_t>::min();
      for (int k = j; k <= i; ++k) {
        running_sum += vals[k];
        min = std::min(min, vals[k]);
        max = std::max(max, vals[k]);
      }
      if (running_sum == search_sum) {
        return min + max;
      }
    }
  }
  return absl::InvalidArgumentError("Not found");
}

absl::StatusOr<std::vector<std::string>> Day09_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int64_t>> vals = ParseAsInts(input);
  if (!vals.ok()) return vals.status();
  return IntReturn(FindMissingXMASPair(*vals));
}

absl::StatusOr<std::vector<std::string>> Day09_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int64_t>> vals = ParseAsInts(input);
  if (!vals.ok()) return vals.status();
  absl::StatusOr<int> missing = FindMissingXMASPair(*vals);
  if (!missing.ok()) return missing.status();
  return IntReturn(FindContiguousRangeMinMaxSum(*vals, *missing));
}

#include "advent_of_code/2020/day09/day09.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

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

}  // namespace

absl::StatusOr<std::string> Day_2020_09::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> vals, ParseAsInts(input));
  return AdventReturn(FindMissingXMASPair(vals));
}

absl::StatusOr<std::string> Day_2020_09::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> vals, ParseAsInts(input));
  ASSIGN_OR_RETURN(int missing, FindMissingXMASPair(vals));
  return AdventReturn(FindContiguousRangeMinMaxSum(vals, missing));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/9,
    []() { return std::unique_ptr<AdventDay>(new Day_2020_09()); });

}  // namespace advent_of_code

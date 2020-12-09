#include "advent_of_code/2020/day9/day9.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day9_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  std::vector<int64_t> vals;
  for (int i = 0; i < input.size(); ++i) {
    int64_t v;
    if (!absl::SimpleAtoi(input[i], &v)) return absl::InvalidArgumentError("parse");
    vals.push_back(v);
  }
  absl::flat_hash_set<int64_t> hist;
  if (vals.size() < 26) return absl::InvalidArgumentError("Too short");
  for (int i = 0; i < 25; ++i) {
    hist.insert(vals[i]);
  }
  for (int i = 25; i < input.size(); ++i) {
    bool found = false;
    for (int64_t v : hist) {
      if (vals[i] - v != v && hist.contains(vals[i] - v)) {
        found = true;
        break;
      }
    }
    if (!found) {
      return IntReturn(vals[i]);
    }
    hist.insert(vals[i]);
    hist.erase(vals[i - 25]);
  }
  return absl::InvalidArgumentError("Not found");
}

absl::StatusOr<std::vector<std::string>> Day9_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  return IntReturn(-1);
}

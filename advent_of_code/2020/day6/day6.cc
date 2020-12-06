#include "advent_of_code/2020/day6/day6.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day6_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  int group_sum = 0;
  absl::flat_hash_set<char> hist;
  for (absl::string_view str : input) {
    if (str.empty()) {
      group_sum += hist.size();
      hist.clear();
    }
    for (char c : str) {
      hist.insert(c);
    }
  }
  group_sum += hist.size();
  return IntReturn(group_sum);
}

absl::StatusOr<std::vector<std::string>> Day6_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  int group_sum = 0;
  absl::flat_hash_map<char, int> hist;
  int group_size = 0;
  for (absl::string_view str : input) {
    if (str.empty()) {
      int all_yes = 0;
      for (const auto& pair : hist) {
        if (pair.second == group_size) {
          ++all_yes;
        }
      }
      group_sum += all_yes;
      hist.clear();
      group_size = 0;
    } else {
      for (char c : str) {
        hist[c]++;
      }
      group_size++;
    }
  }
  int all_yes = 0;
  for (const auto& pair : hist) {
    if (pair.second == group_size) {
      ++all_yes;
    }
  }
  group_sum += all_yes;
  return IntReturn(group_sum);
}

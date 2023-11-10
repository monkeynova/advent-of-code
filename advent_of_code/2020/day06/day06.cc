#include "advent_of_code/2020/day06/day06.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_06::Part1(
    absl::Span<std::string_view> input) const {
  int group_sum = 0;
  absl::flat_hash_set<char> hist;
  for (std::string_view str : input) {
    if (str.empty()) {
      group_sum += hist.size();
      hist.clear();
    }
    for (char c : str) {
      hist.insert(c);
    }
  }
  group_sum += hist.size();
  return AdventReturn(group_sum);
}

absl::StatusOr<std::string> Day_2020_06::Part2(
    absl::Span<std::string_view> input) const {
  int group_sum = 0;
  absl::flat_hash_map<char, int> hist;
  int group_size = 0;
  for (std::string_view str : input) {
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
  return AdventReturn(group_sum);
}

}  // namespace advent_of_code

// http://adventofcode.com/2022/day/03

#include "advent_of_code/2022/day03/day03.h"

#include "absl/algorithm/container.h"
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

absl::StatusOr<std::string> Day_2022_03::Part1(
    absl::Span<absl::string_view> input) const {
  int score = 0;
  for (absl::string_view line : input) {
    std::vector<int> counts(256);
    for (int i = 0; i < line.size() / 2; ++i) {
      ++counts[line[i]];
    }
    for (int i = line.size() / 2; i < line.size(); ++i) {
      if (counts[line[i]]) {
        if (line[i] >= 'A' && line[i] <= 'Z')
          score += line[i] - 'A' + 27;
        else if (line[i] >= 'a' && line[i] <= 'z')
          score += line[i] - 'a' + 1;
        else Error("Bad line");
        break;
      }
    }
  }
  return IntReturn(score);
}

absl::StatusOr<std::string> Day_2022_03::Part2(
    absl::Span<absl::string_view> input) const {
  int score = 0;
  for (int i = 0; i < input.size(); i += 3) {
    absl::flat_hash_set<char> a_set;
    absl::flat_hash_set<char> b_set;
    absl::flat_hash_set<char> c_set;
    for (int j = 0; j < input[i].size(); ++j) {
      a_set.insert(input[i][j]);
    }
    for (int j = 0; j < input[i+1].size(); ++j) {
      b_set.insert(input[i+1][j]);
    }
    for (int j = 0; j < input[i+2].size(); ++j) {
      c_set.insert(input[i+2][j]);
    }
    for (char c : a_set) {
      if (b_set.contains(c) && c_set.contains(c)) {
        if (c >= 'A' && c <= 'Z')
          score += c - 'A' + 27;
        else if (c >= 'a' && c <= 'z')
          score += c - 'a' + 1;
        else Error("Bad line");
        break;
      }
    }
  }
  return IntReturn(score);
}

}  // namespace advent_of_code

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

absl::StatusOr<std::string> Day_2022_03::Part1(
    absl::Span<absl::string_view> input) const {
  int score = 0;
  for (absl::string_view line : input) {
    if (line.size() % 2 != 0) return Error("Line size isn't even");

    std::vector<int> counts(256, 0);
    for (int i = 0; i < line.size() / 2; ++i) {
      ++counts[line[i]];
    }
    std::optional<char> found;
    for (int i = line.size() / 2; i < line.size(); ++i) {
      if (counts[line[i]]) {
        if (found) {
          if (*found != line[i]) {
            return Error("Double result in a line: \"", line, "\"");
          }
          continue;
        }
        found = line[i];
        if (line[i] >= 'A' && line[i] <= 'Z') score += line[i] - 'A' + 27;
        else if (line[i] >= 'a' && line[i] <= 'z') score += line[i] - 'a' + 1;
        else return Error("Bad char");
      }
    }
    if (!found) return Error("Bad line: no duplicate");
  }
  return IntReturn(score);
}

absl::StatusOr<std::string> Day_2022_03::Part2(
    absl::Span<absl::string_view> input) const {
  int score = 0;
  if (input.size() % 3 != 0) return Error("Input isn't groups of 3");
  for (int i = 0; i < input.size(); i += 3) {
    absl::flat_hash_set<char> a_set;
    absl::flat_hash_set<char> b_set;
    for (char c : input[i]) a_set.insert(c);
    for (char c : input[i+1]) b_set.insert(c);
    std::optional<char> found;
    for (char c : input[i+2]) {
      if (a_set.contains(c) && b_set.contains(c)) {
        if (found) {
          if (*found != c) {
            return Error("Multiple duplicates");
          }
          continue;
        }
        found = c;
        if (c >= 'A' && c <= 'Z') score += c - 'A' + 27;
        else if (c >= 'a' && c <= 'z') score += c - 'a' + 1;
        else return Error("Bad line");
      }
    }
    if (!found) return Error("Bad set: no duplicate");
  }
  return IntReturn(score);
}

}  // namespace advent_of_code

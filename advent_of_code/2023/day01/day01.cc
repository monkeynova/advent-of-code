// http://adventofcode.com/2023/day/01

#include "advent_of_code/2023/day01/day01.h"

#include "absl/algorithm/container.h"
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

absl::StatusOr<std::string> Day_2023_01::Part1(
    absl::Span<std::string_view> input) const {
  int sum = 0;
  for (std::string_view line : input) {
    int first = -1;
    int last = -1;
    for (char c : line) {
      if (c >= '0' && c <= '9') {
        if (first == -1) first = c - '0';
        last = c - '0';
      }
    }
    sum += first * 10 + last;
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2023_01::Part2(
    absl::Span<std::string_view> input) const {
  std::array<std::string_view, 10> words = {
    "zero", "one", "two", "three", "four", "five", "six", "seven",
    "eight", "nine",
  };
  int sum = 0;
  for (std::string_view line : input) {
    int first = -1;
    int last = -1;
    for (int i = 0; i < line.size(); ++i) {
      for (int d = 0; d < 10; ++d) {
        std::string_view digit_word = words[d];
        if (line.substr(i, digit_word.size()) == digit_word) {
          if (first == -1) first = d;
          last = d;
        }
      }
      if (line[i] >= '0' && line[i] <= '9') {
        if (first == -1) first = line[i] - '0';
        last = line[i] - '0';
      }
    }
    VLOG(1) << line << ": " << first << "-" << last;
    sum += first * 10 + last;
  }
  return AdventReturn(sum);
}

}  // namespace advent_of_code

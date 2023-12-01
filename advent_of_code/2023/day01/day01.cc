// http://adventofcode.com/2023/day/01

#include "advent_of_code/2023/day01/day01.h"

#include "absl/log/log.h"

namespace advent_of_code {

std::optional<int> TryParse(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  return std::nullopt;
}

std::optional<int> TryParse(std::string_view prefix) {
  static constexpr std::array<std::string_view, 10> words = {
    "zero", "one", "two", "three", "four", "five", "six", "seven", "eight",
    "nine",
  };
  if (prefix[0] >= '0' && prefix[0] <= '9') {
    return prefix[0] - '0';
  }
  for (int d = 0; d < 10; ++d) {
    std::string_view digit_word = words[d];
    if (prefix.substr(0, digit_word.size()) == digit_word) {
      return d;
    }
  }
  return std::nullopt;
}

absl::StatusOr<std::string> Day_2023_01::Part1(
    absl::Span<std::string_view> input) const {
  int sum = 0;
  for (std::string_view line : input) {
    std::optional<int> first;
    for (int i = 0; !first && i < line.size(); ++i) {
      first = TryParse(line[i]);
    }
    if (!first) return Error("No digits in: ", line);
    std::optional<int> last;
    for (int i = line.size() - 1; !last && i >= 0; --i) {
      last = TryParse(line[i]);
    }
    if (!last) return Error("No digits in: ", line);
    sum += *first * 10 + *last;
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2023_01::Part2(
    absl::Span<std::string_view> input) const {
  int sum = 0;
  for (std::string_view line : input) {
    std::optional<int> first;
    for (int i = 0; !first && i < line.size(); ++i) {
      first = TryParse(line.substr(i));
    }
    if (!first) return Error("No digits in: ", line);
    std::optional<int> last;
    for (int i = line.size() - 1; !last && i >= 0; --i) {
      last = TryParse(line.substr(i));
    }
    if (!last) return Error("No digits in: ", line);
    sum += *first * 10 + *last;
  }
  return AdventReturn(sum);
}

}  // namespace advent_of_code

// http://adventofcode.com/2023/day/1

#include "advent_of_code/2023/day01/day01.h"

#include "absl/log/log.h"

namespace advent_of_code {

namespace {

std::optional<int> TryParse(char c) {
  switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
  }
  return std::nullopt;
}

std::optional<int> TryParse(std::string_view prefix) {
  switch (prefix[0]) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'z': {
      if (prefix.substr(0, 4) == "zero") return 0;
      break;
    }
    case 'o': {
      if (prefix.substr(0, 3) == "one") return 1;
      break;
    }
    case 't': {
      if (prefix.substr(0, 3) == "two") return 2;
      if (prefix.substr(0, 5) == "three") return 3;
      break;
    }
    case 'f': {
      if (prefix.substr(0, 4) == "four") return 4;
      if (prefix.substr(0, 4) == "five") return 5;
      break;
    }
    case 's': {
      if (prefix.substr(0, 3) == "six") return 6;
      if (prefix.substr(0, 5) == "seven") return 7;
      break;
    }
    case 'e': {
      if (prefix.substr(0, 5) == "eight") return 8;
      break;
    }
    case 'n': {
      if (prefix.substr(0, 4) == "nine") return 9;
      break;
    }
  }
  return std::nullopt;
}

}  // namespace

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

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/1, []() {
  return std::unique_ptr<AdventDay>(new Day_2023_01());
});

}  // namespace advent_of_code

// http://adventofcode.com/2024/day/1

#include "advent_of_code/2024/day01/day01.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_01::Part1(
    absl::Span<std::string_view> input) const {
  std::vector<int> l1, l2;
  for (std::string_view line : input) {
    Tokenizer t(line);
    ASSIGN_OR_RETURN(int i1, t.NextInt());
    ASSIGN_OR_RETURN(int i2, t.NextInt());
    if (!t.Done()) {
      return absl::InvalidArgumentError(absl::StrCat("Bad line: ", line));
    }
    l1.push_back(i1);
    l2.push_back(i2);
  }
  absl::c_sort(l1);
  absl::c_sort(l2);
  int sum = 0;
  for (int i = 0; i < l1.size(); ++i) {
    sum += std::abs(l1[i] - l2[i]);
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_01::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<int> l1, l2;
  for (std::string_view line : input) {
    Tokenizer t(line);
    ASSIGN_OR_RETURN(int i1, t.NextInt());
    ASSIGN_OR_RETURN(int i2, t.NextInt());
    if (!t.Done()) {
      return absl::InvalidArgumentError(absl::StrCat("Bad line: ", line));
    }
    l1.push_back(i1);
    l2.push_back(i2);
  }
  absl::c_sort(l1);
  absl::c_sort(l2);

  int sum = 0;

  int i2 = 0;
  int run_size = 1;
  if (l1.empty()) return AdventReturn(0);
  for (int i1 = 1; i1 < l1.size(); ++i1) {
    if (l1[i1] == l1[i1 - 1]) {
      ++run_size;
    } else {
      for (/*nop*/; i2 < l2.size(); ++i2) {
        if (l2[i2] < l1[i1 - 1]) continue;
        if (l1[i1 - 1] != l2[i2]) break;
        sum += l1[i1 - 1] * run_size;
      }
      run_size = 1;
    }
  }
  for (/*nop*/; i2 < l2.size(); ++i2) {
    if (l2[i2] < l1.back()) continue;
    if (l1.back() != l2[i2]) break;
    sum += l1.back() * run_size;
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/1,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_01()); });

}  // namespace advent_of_code

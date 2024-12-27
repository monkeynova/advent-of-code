// http://adventofcode.com/2024/day/7

#include "advent_of_code/2024/day07/day07.h"

#include "absl/log/log.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

bool CanMakeAddMul(int64_t test, absl::Span<const int64_t> vals) {
  if (vals.size() == 1) return test == vals[0];

  if (test < vals.back()) return false;
  absl::Span<const int64_t> tail = vals.subspan(0, vals.size() - 1);

  if (test % vals.back() == 0 && CanMakeAddMul(test / vals.back(), tail)) {
    return true;
  }
  return CanMakeAddMul(test - vals.back(), tail);
}

bool CanMakeAddMulCat(int64_t test, absl::Span<const int64_t> vals) {
  if (vals.size() == 1) return test == vals[0];

  if (test < vals.back()) return false;
  absl::Span<const int64_t> tail = vals.subspan(0, vals.size() - 1);

  int64_t pow10 = 1;
  while (pow10 <= vals.back()) pow10 *= 10;
  if (test % pow10 == vals.back() && CanMakeAddMulCat(test / pow10, tail)) {
    return true;
  }
  if (test % vals.back() == 0 && CanMakeAddMulCat(test / vals.back(), tail)) {
    return true;
  }
  return CanMakeAddMulCat(test - vals.back(), tail);
}


}  // namespace

absl::StatusOr<std::string> Day_2024_07::Part1(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view line : input) {
    Tokenizer t(line);
    ASSIGN_OR_RETURN(int64_t test, t.NextInt());
    RETURN_IF_ERROR(t.NextIs(":"));
    std::vector<int64_t> vals;
    while (!t.Done()) {
      ASSIGN_OR_RETURN(int64_t val, t.NextInt());
      if (val < 0) return absl::UnimplementedError("Negative value");
      vals.push_back(val);
    }
    if (CanMakeAddMul(test, vals)) {
      sum += test;
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_07::Part2(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view line : input) {
    Tokenizer t(line);
    ASSIGN_OR_RETURN(int64_t test, t.NextInt());
    RETURN_IF_ERROR(t.NextIs(":"));
    std::vector<int64_t> vals;
    while (!t.Done()) {
      ASSIGN_OR_RETURN(int64_t val, t.NextInt());
      if (val < 0) return absl::UnimplementedError("Negative value");
      vals.push_back(val);
    }
    if (CanMakeAddMulCat(test, vals)) {
      sum += test;
    }
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/7,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_07()); });

}  // namespace advent_of_code

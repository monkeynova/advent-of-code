// http://adventofcode.com/2019/day/04

#include "advent_of_code/2019/day04/day04.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"

namespace advent_of_code {
namespace {

void AdvancePassword(std::string& password) {
  for (int idx = password.size() - 1; idx >= 0; --idx) {
    ++password[idx];
    if (password[idx] != '9' + 1) break;
    password[idx] = '0';
  }
}

bool IsValidPassword(absl::string_view password) {
  if (password.size() != 6) return false;
  bool has_pair = false;
  for (int i = 0; i < 5; ++i) {
    if (password[i + 1] < password[i]) return false;
    if (password[i + 1] == password[i]) has_pair = true;
  }
  return has_pair;
}

bool IsValidPassword2(absl::string_view password) {
  if (password.size() != 6) return false;

  bool has_pair = false;
  for (int i = 0; i < 5; ++i) {
    if (password[i + 1] < password[i]) return false;
    if (password[i + 1] == password[i] &&
        (i == 4 || password[i] != password[i + 2]) &&
        (i == 0 || password[i] != password[i - 1])) {
      has_pair = true;
    }
  }
  return has_pair;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_04::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Empty input");
  const auto [min, max] = PairSplit(input[0], "-");
  if (min.size() != 6) return absl::InvalidArgumentError("Bad min");
  if (max.size() != 6) return absl::InvalidArgumentError("Bad max");

  int valid = 0;
  for (std::string password = std::string(min); password <= max;
       AdvancePassword(password)) {
    if (IsValidPassword(password)) ++valid;
  }

  return IntReturn(valid);
}

absl::StatusOr<std::string> Day_2019_04::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Empty input");
  const auto [min, max] = PairSplit(input[0], "-");
  if (min.size() != 6) return absl::InvalidArgumentError("Bad min");
  if (max.size() != 6) return absl::InvalidArgumentError("Bad max");

  int valid = 0;
  for (std::string password = std::string(min); password <= max;
       AdvancePassword(password)) {
    if (IsValidPassword2(password)) ++valid;
  }

  return IntReturn(valid);
}

}  // namespace advent_of_code

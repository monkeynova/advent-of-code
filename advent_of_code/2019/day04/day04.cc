#include "advent_of_code/2019/day04/day04.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

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

absl::StatusOr<std::vector<std::string>> Day04_2019::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Empty input");
  std::vector<absl::string_view> range = absl::StrSplit(input[0], "-");
  if (range.size() != 2) return absl::InvalidArgumentError("Bad range");
  if (range[0].size() != 6) return absl::InvalidArgumentError("Bad min");
  if (range[1].size() != 6) return absl::InvalidArgumentError("Bad max");

  int valid = 0;
  for (std::string password = std::string(range[0]); password <= range[1];
       AdvancePassword(password)) {
    if (IsValidPassword(password)) ++valid;
  }

  return IntReturn(valid);
}

absl::StatusOr<std::vector<std::string>> Day04_2019::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Empty input");
  std::vector<absl::string_view> range = absl::StrSplit(input[0], "-");
  if (range.size() != 2) return absl::InvalidArgumentError("Bad range");
  if (range[0].size() != 6) return absl::InvalidArgumentError("Bad min");
  if (range[1].size() != 6) return absl::InvalidArgumentError("Bad max");

  int valid = 0;
  for (std::string password = std::string(range[0]); password <= range[1];
       AdvancePassword(password)) {
    if (IsValidPassword2(password)) ++valid;
  }

  return IntReturn(valid);
}

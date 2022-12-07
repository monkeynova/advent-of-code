// https://adventofcode.com/2019/day/4
//
// --- Day 4: Secure Container ---
// -------------------------------
// 
// You arrive at the Venus fuel depot only to discover it's protected by
// a password. The Elves had written the password on a sticky note, but
// someone threw it out.
// 
// However, they do remember a few key facts about the password:
// 
// * It is a six-digit number.
// 
// * The value is within the range given in your puzzle input.
// 
// * Two adjacent digits are the same (like 22 in 122345).
// 
// * Going from left to right, the digits never decrease; they only
// ever increase or stay the same (like 111123 or 135679).
// 
// Other than the range rule, the following are true:
// 
// * 111111 meets these criteria (double 11, never decreases).
// 
// * 223450 does not meet these criteria (decreasing pair of digits 50).
// 
// * 123789 does not meet these criteria (no double).
// 
// How many different passwords within the range given in your puzzle
// input meet these criteria?
//
// --- Part Two ---
// ----------------
// 
// An Elf just remembered one more important detail: the two adjacent
// matching digits are not part of a larger group of matching digits.
// 
// Given this additional criterion, but still ignoring the range rule,
// the following are now true:
// 
// * 112233 meets these criteria because the digits never decrease and
// all repeated digits are exactly two digits long.
// 
// * 123444 no longer meets the criteria (the repeated 44 is part of a
// larger group of 444).
// 
// * 111122 meets the criteria (even though 1 is repeated more than
// twice, it still contains a double 22).
// 
// How many different passwords within the range given in your puzzle
// input meet all of the criteria?


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

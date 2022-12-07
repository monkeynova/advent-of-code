// https://adventofcode.com/2017/day/2
//
// --- Day 2: Corruption Checksum ---
// ----------------------------------
//
// As you walk through the door, a glowing humanoid shape yells in your
// direction. "You there! Your state appears to be idle. Come help us
// repair the corruption in this spreadsheet - if we take another
// millisecond, we'll have to display an hourglass cursor!"
//
// The spreadsheet consists of rows of apparently-random numbers. To make
// sure the recovery process is on the right track, they need you to
// calculate the spreadsheet's checksum. For each row, determine the
// difference between the largest value and the smallest value; the
// checksum is the sum of all of these differences.
//
// For example, given the following spreadsheet:
//
// 5 1 9 5
// 7 5 3
// 2 4 6 8
//
// * The first row's largest and smallest values are 9 and 1, and their
// difference is 8.
//
// * The second row's largest and smallest values are 7 and 3, and
// their difference is 4.
//
// * The third row's difference is 6.
//
// In this example, the spreadsheet's checksum would be 8 + 4 + 6 = 18.
//
// What is the checksum for the spreadsheet in your puzzle input?
//
// --- Part Two ---
// ----------------
//
// "Great work; looks like we're on the right track after all. Here's a
// star for your effort." However, the program seems a little worried.
// Can programs be worried?
//
// "Based on what we're seeing, it looks like all the User wanted is some
// information about the evenly divisible values in the spreadsheet.
// Unfortunately, none of us are equipped for that kind of calculation -
// most of us specialize in bitwise operations."
//
// It sounds like the goal is to find the only two numbers in each row
// where one evenly divides the other - that is, where the result of the
// division operation is a whole number. They would like you to find
// those numbers on each line, divide them, and add up each line's
// result.
//
// For example, given the following spreadsheet:
//
// 5 9 2 8
// 9 4 7 3
// 3 8 6 5
//
// * In the first row, the only two numbers that evenly divide are 8
// and 2; the result of this division is 4.
//
// * In the second row, the two numbers are 9 and 3; the result is 3.
//
// * In the third row, the result is 2.
//
// In this example, the sum of the results would be 4 + 3 + 2 = 9.
//
// What is the sum of each row's result in your puzzle input?

#include "advent_of_code/2017/day02/day02.h"

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

absl::StatusOr<std::string> Day_2017_02::Part1(
    absl::Span<absl::string_view> input) const {
  int checksum = 0;
  for (absl::string_view row : input) {
    std::vector<absl::string_view> nums = absl::StrSplit(row, "\t");
    int max = std::numeric_limits<int>::min();
    int min = std::numeric_limits<int>::max();
    for (absl::string_view num_str : nums) {
      int num;
      if (!absl::SimpleAtoi(num_str, &num)) return Error("Bad num: ", num_str);
      max = std::max(max, num);
      min = std::min(min, num);
    }
    checksum += max - min;
  }
  return IntReturn(checksum);
}

absl::StatusOr<std::string> Day_2017_02::Part2(
    absl::Span<absl::string_view> input) const {
  int checksum = 0;
  for (absl::string_view row : input) {
    std::vector<absl::string_view> num_strs = absl::StrSplit(row, "\t");
    absl::StatusOr<std::vector<int64_t>> nums = ParseAsInts(num_strs);
    if (!nums.ok()) return nums.status();
    bool found = false;
    for (int64_t a : *nums) {
      for (int64_t b : *nums) {
        if (a <= b) continue;
        if (a % b == 0) {
          if (found) return Error("Dual divisible: ", row);
          found = true;
          checksum += a / b;
        }
      }
    }
    if (!found) return Error("No divisors found: ", row);
  }
  return IntReturn(checksum);
}

}  // namespace advent_of_code

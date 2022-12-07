// https://adventofcode.com/2015/day/10
//
// --- Day 10: Elves Look, Elves Say ---
// -------------------------------------
//
// Today, the Elves are playing a game called look-and-say. They take
// turns making sequences by reading aloud the previous sequence and
// using that reading as the next sequence. For example, 211 is read as
// "one two, two ones", which becomes 1221 (1 2, 2 1s).
//
// Look-and-say sequences are generated iteratively, using the previous
// value as input for the next step. For each step, take the previous
// value, and replace each run of digits (like 111) with the number of
// digits (3) followed by the digit itself (1).
//
// For example:
//
// * 1 becomes 11 (1 copy of digit 1).
//
// * 11 becomes 21 (2 copies of digit 1).
//
// * 21 becomes 1211 (one 2 followed by one 1).
//
// * 1211 becomes 111221 (one 1, one 2, and two 1s).
//
// * 111221 becomes 312211 (three 1s, two 2s, and one 1).
//
// Starting with the digits in your puzzle input, apply this process 40
// times. What is the length of the result?
//
// --- Part Two ---
// ----------------
//
// Neat, right? You might also enjoy hearing John Conway talking about
// this sequence (that's Conway of Conway's Game of Life fame).
//
// Now, starting again with the digits in your puzzle input, apply this
// process 50 times. What is the length of the new result?

#include "advent_of_code/2015/day10/day10.h"

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

std::string LookAndSay(absl::string_view input) {
  std::string out;
  for (int i = 0; i < input.size();) {
    int j;
    for (j = i; j < input.size() && input[j] == input[i]; ++j) {
      // Spin.
    }
    out.append(absl::StrCat(j - i, input.substr(i, 1)));
    i = j;
  }
  return out;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_10::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string str = std::string(input[0]);
  for (int i = 0; i < 40; ++i) {
    VLOG(1) << str;
    str = LookAndSay(str);
  }
  return IntReturn(str.size());
}

absl::StatusOr<std::string> Day_2015_10::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string str = std::string(input[0]);
  for (int i = 0; i < 50; ++i) {
    VLOG(1) << str;
    str = LookAndSay(str);
  }
  return IntReturn(str.size());
}

}  // namespace advent_of_code

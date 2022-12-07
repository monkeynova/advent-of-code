// https://adventofcode.com/2015/day/1
//
// --- Day 1: Not Quite Lisp ---
// -----------------------------
//
// Santa was hoping for a white Christmas, but his weather machine's
// "snow" function is powered by stars, and he's fresh out! To save
// Christmas, he needs you to collect fifty stars by December 25th.
//
// Collect stars by helping Santa solve puzzles. Two puzzles will be made
// available on each day in the Advent calendar; the second puzzle is
// unlocked when you complete the first. Each puzzle grants one star.
// Good luck!
//
// Here's an easy puzzle to warm you up.
//
// Santa is trying to deliver presents in a large apartment building, but
// he can't find the right floor - the directions he got are a little
// confusing. He starts on the ground floor (floor 0) and then follows
// the instructions one character at a time.
//
// An opening parenthesis, (, means he should go up one floor, and a
// closing parenthesis, ), means he should go down one floor.
//
// The apartment building is very tall, and the basement is very deep; he
// will never find the top or bottom floors.
//
// For example:
//
// * (()) and ()() both result in floor 0.
//
// * ((( and (()(()( both result in floor 3.
//
// * ))((((( also results in floor 3.
//
// * ()) and ))( both result in floor -1 (the first basement level).
//
// * ))) and )())()) both result in floor -3.
//
// To what floor do the instructions take Santa?
//
// --- Part Two ---
// ----------------
//
// Now, given the same instructions, find the position of the first
// character that causes him to enter the basement (floor -1). The first
// character in the instructions has position 1, the second character has
// position 2, and so on.
//
// For example:
//
// * ) causes him to enter the basement at character position 1.
//
// * ()()) causes him to enter the basement at character position 5.
//
// What is the position of the character that causes Santa to first enter
// the basement?

#include "advent_of_code/2015/day01/day01.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_01::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  int floor = 0;
  for (char c : input[0]) {
    if (c == '(')
      ++floor;
    else if (c == ')')
      --floor;
    else
      return absl::InvalidArgumentError("Bad char");
  }
  return IntReturn(floor);
}

absl::StatusOr<std::string> Day_2015_01::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  int floor = 0;
  for (int i = 0; i < input[0].size(); ++i) {
    char c = input[0][i];
    if (c == '(')
      ++floor;
    else if (c == ')')
      --floor;
    else
      return absl::InvalidArgumentError("Bad char");
    if (floor < 0) return IntReturn(i + 1);
  }
  return absl::InvalidArgumentError("No basement");
}

}  // namespace advent_of_code

// https://adventofcode.com/2017/day/8
//
// --- Day 8: I Heard You Like Registers ---
// -----------------------------------------
// 
// You receive a signal directly from the CPU. Because of your recent
// assistance with jump instructions, it would like you to compute the
// result of a series of unusual register instructions.
// 
// Each instruction consists of several parts: the register to modify,
// whether to increase or decrease that register's value, the amount by
// which to increase or decrease it, and a condition. If the condition
// fails, skip the instruction without modifying the register. The
// registers all start at 0. The instructions look like this:
// 
// b inc 5 if a > 1
// a inc 1 if b < 5
// c dec -10 if a >= 1
// c inc -20 if c == 10
// 
// These instructions would be processed as follows:
// 
// * Because a starts at 0, it is not greater than 1, and so b is not
// modified.
// 
// * a is increased by 1 (to 1) because b is less than 5 (it is 0).
// 
// * c is decreased by -10 (to 10) because a is now greater than or
// equal to 1 (it is 1).
// 
// * c is increased by -20 (to -10) because c is equal to 10.
// 
// After this process, the largest value in any register is 1.
// 
// You might also encounter <= (less than or equal to) or != (not equal
// to). However, the CPU doesn't have the bandwidth to tell you what all
// the registers are named, and leaves that to you to determine.
// 
// What is the largest value in any register after completing the
// instructions in your puzzle input?
//
// --- Part Two ---
// ----------------
// 
// To be safe, the CPU also needs to know the highest value held in any
// register during this process so that it can decide how much memory to
// allocate to these operations. For example, in the above instructions,
// the highest value ever held was 10 (in register c after the third
// instruction was evaluated).


#include "advent_of_code/2017/day08/day08.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_08::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, int> registers;
  for (absl::string_view in : input) {
    const auto [op, cond] = PairSplit(in, " if ");
    absl::string_view reg;
    absl::string_view fn;
    int num;
    if (!RE2::FullMatch(cond, "([a-z]+) (<|>|<=|>=|==|!=) (-?\\d+)", &reg, &fn,
                        &num)) {
      return Error("Bad cond: ", cond);
    }
    if (fn == "==") {
      if (registers[reg] != num) continue;
    } else if (fn == "!=") {
      if (registers[reg] == num) continue;
    } else if (fn == "<") {
      if (registers[reg] >= num) continue;
    } else if (fn == ">") {
      if (registers[reg] <= num) continue;
    } else if (fn == ">=") {
      if (registers[reg] < num) continue;
    } else if (fn == "<=") {
      if (registers[reg] > num) continue;
    } else {
      return Error("Bad fn: ", fn);
    }

    if (!RE2::FullMatch(op, "([a-z]+) (inc|dec) (-?\\d+)", &reg, &fn, &num)) {
      return Error("Bad op: ", op);
    }
    if (fn == "inc") {
      registers[reg] += num;
    } else if (fn == "dec") {
      registers[reg] -= num;
    } else {
      return Error("Bad fn: ", fn);
    }
  }
  int max = std::numeric_limits<int>::min();
  for (const auto& [_, val] : registers) max = std::max(max, val);
  return IntReturn(max);
}

absl::StatusOr<std::string> Day_2017_08::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, int> registers;
  int max = std::numeric_limits<int>::min();
  for (absl::string_view in : input) {
    const auto [op, cond] = PairSplit(in, " if ");
    absl::string_view reg;
    absl::string_view fn;
    int num;
    if (!RE2::FullMatch(cond, "([a-z]+) (<|>|<=|>=|==|!=) (-?\\d+)", &reg, &fn,
                        &num)) {
      return Error("Bad cond: ", cond);
    }
    if (fn == "==") {
      if (registers[reg] != num) continue;
    } else if (fn == "!=") {
      if (registers[reg] == num) continue;
    } else if (fn == "<") {
      if (registers[reg] >= num) continue;
    } else if (fn == ">") {
      if (registers[reg] <= num) continue;
    } else if (fn == ">=") {
      if (registers[reg] < num) continue;
    } else if (fn == "<=") {
      if (registers[reg] > num) continue;
    } else {
      return Error("Bad fn: ", fn);
    }

    if (!RE2::FullMatch(op, "([a-z]+) (inc|dec) (-?\\d+)", &reg, &fn, &num)) {
      return Error("Bad op: ", op);
    }
    if (fn == "inc") {
      registers[reg] += num;
      max = std::max(max, registers[reg]);
    } else if (fn == "dec") {
      registers[reg] -= num;
      max = std::max(max, registers[reg]);
    } else {
      return Error("Bad fn: ", fn);
    }
  }
  return IntReturn(max);
}

}  // namespace advent_of_code

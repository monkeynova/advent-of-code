#include "advent_of_code/2017/day08/day08.h"

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
  return AdventReturn(max);
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
  return AdventReturn(max);
}

}  // namespace advent_of_code

#include "advent_of_code/2017/day08/day08.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day08_2017::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, int> registers;
  for (absl::string_view in : input) {
    std::vector<absl::string_view> op_and_cond = absl::StrSplit(in, " if ");
    if (op_and_cond.size() != 2) return Error("Bad if");
    absl::string_view reg;
    absl::string_view op;
    int num;
    if (!RE2::FullMatch(op_and_cond[1], "([a-z]+) (<|>|<=|>=|==|!=) (-?\\d+)", &reg, &op, &num)) {
      return Error("Bad cond: ", op_and_cond[1]);
    }
    if (op == "==") { if (registers[reg] != num) continue; }
    else if (op == "!=") { if (registers[reg] == num) continue; }
    else if (op == "<") { if (registers[reg] >= num) continue; }
    else if (op == ">") { if (registers[reg] <= num) continue; }
    else if (op == ">=") { if (registers[reg] < num) continue; }
    else if (op == "<=") { if (registers[reg] > num) continue; }
    else return Error("Bad cond: ", op);

    if (!RE2::FullMatch(op_and_cond[0], "([a-z]+) (inc|dec) (-?\\d+)", &reg, &op, &num)) {
      return Error("Bad op: ", op_and_cond[0]);
    }
    if (op == "inc") registers[reg] += num;
    else if (op == "dec") registers[reg] -= num;
    else return Error("Bad op: ", op);
  }
  int max = std::numeric_limits<int>::min();
  for (const auto& [_, val] : registers) max = std::max(max, val);
  return IntReturn(max);
}

absl::StatusOr<std::vector<std::string>> Day08_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

#include "advent_of_code/2015/day07/day07.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

struct Operation {
  std::string operation;
  std::string arg1;
  std::string arg2;
  std::string dest;
};

absl::StatusOr<int> EvaluateMemo(
    absl::flat_hash_map<absl::string_view, int>* memo,
    const absl::flat_hash_map<std::string, Operation>& ops_by_dest,
    absl::string_view dest) {
  if (auto it = memo->find(dest); it != memo->end()) return it->second;

  int ret;
  if (!absl::SimpleAtoi(dest, &ret)) {
    auto it = ops_by_dest.find(dest);
    if (it == ops_by_dest.end()) {
      return AdventDay::Error("No ", dest);
    }
    const Operation& op = it->second;
    if (op.operation == "") {
      absl::StatusOr<int> in = EvaluateMemo(memo, ops_by_dest, op.arg1);
      if (!in.ok()) return in.status();
      ret = *in;
    } else if (op.operation == "NOT") {
      absl::StatusOr<int> in = EvaluateMemo(memo, ops_by_dest, op.arg1);
      if (!in.ok()) return in.status();
      ret = ~*in;
    } else if (op.operation == "AND") {
      absl::StatusOr<int> in1 = EvaluateMemo(memo, ops_by_dest, op.arg1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int> in2 = EvaluateMemo(memo, ops_by_dest, op.arg2);
      if (!in2.ok()) return in2.status();
      ret = *in1 & *in2;
    } else if (op.operation == "OR") {
      absl::StatusOr<int> in1 = EvaluateMemo(memo, ops_by_dest, op.arg1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int> in2 = EvaluateMemo(memo, ops_by_dest, op.arg2);
      if (!in2.ok()) return in2.status();
      ret = *in1 | *in2;
    } else if (op.operation == "LSHIFT") {
      absl::StatusOr<int> in1 = EvaluateMemo(memo, ops_by_dest, op.arg1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int> in2 = EvaluateMemo(memo, ops_by_dest, op.arg2);
      if (!in2.ok()) return in2.status();
      ret = *in1 << *in2;
    } else if (op.operation == "RSHIFT") {
      absl::StatusOr<int> in1 = EvaluateMemo(memo, ops_by_dest, op.arg1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int> in2 = EvaluateMemo(memo, ops_by_dest, op.arg2);
      if (!in2.ok()) return in2.status();
      ret = *in1 >> *in2;
    }
  }

  (*memo)[dest] = ret;
  return ret;
}

absl::StatusOr<int> Evaluate(
    const absl::flat_hash_map<std::string, Operation>& ops_by_dest,
    absl::string_view dest) {
  absl::flat_hash_map<absl::string_view, int> memo;
  return EvaluateMemo(&memo, ops_by_dest, dest);
}

absl::StatusOr<absl::flat_hash_map<std::string, Operation>> Parse(
    const std::vector<absl::string_view>& input) {
  absl::flat_hash_map<std::string, Operation> ops_by_dest;

  for (absl::string_view str : input) {
    std::vector<absl::string_view> op_and_dest = absl::StrSplit(str, " -> ");
    if (op_and_dest.size() != 2) return AdventDay::Error("Bad input: ", str);
    Operation op;
    op.dest = op_and_dest[1];
    if (RE2::FullMatch(op_and_dest[0], "(\\d+|[a-z]+)", &op.arg1)) {
      op.operation = "";
    } else if (RE2::FullMatch(op_and_dest[0], "(\\d+|[a-z]+)", &op.arg1)) {
      op.operation = "";
    } else if (RE2::FullMatch(op_and_dest[0], "NOT (\\d+|[a-z]+)", &op.arg1)) {
      op.operation = "NOT";
    } else if (RE2::FullMatch(op_and_dest[0], "(\\d+|[a-z]+) AND (\\d+|[a-z]+)",
                              &op.arg1, &op.arg2)) {
      op.operation = "AND";
    } else if (RE2::FullMatch(op_and_dest[0], "(\\d+|[a-z]+) OR (\\d+|[a-z]+)",
                              &op.arg1, &op.arg2)) {
      op.operation = "OR";
    } else if (RE2::FullMatch(op_and_dest[0],
                              "(\\d+|[a-z]+) LSHIFT (\\d+|[a-z]+)", &op.arg1,
                              &op.arg2)) {
      op.operation = "LSHIFT";
    } else if (RE2::FullMatch(op_and_dest[0],
                              "(\\d+|[a-z]+) RSHIFT (\\d+|[a-z]+)", &op.arg1,
                              &op.arg2)) {
      op.operation = "RSHIFT";
    } else {
      return AdventDay::Error("Bad op: ", op_and_dest[0]);
    }
    ops_by_dest[op.dest] = op;
  }

  return ops_by_dest;
}

absl::StatusOr<std::vector<std::string>> Day07_2015::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, Operation>> ops_by_dest =
      Parse(input);
  if (!ops_by_dest.ok()) return ops_by_dest.status();
  return IntReturn(Evaluate(*ops_by_dest, "a"));
}

absl::StatusOr<std::vector<std::string>> Day07_2015::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, Operation>> ops_by_dest =
      Parse(input);
  if (!ops_by_dest.ok()) return ops_by_dest.status();
  absl::StatusOr<int> a_val = Evaluate(*ops_by_dest, "a");
  if (!a_val.ok()) return a_val.status();

  (*ops_by_dest)["b"] = Operation{.arg1 = absl::StrCat(*a_val)};
  return IntReturn(Evaluate(*ops_by_dest, "a"));
}

#include "advent_of_code/2015/day07/day07.h"

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

struct Operation {
  std::string operation;
  std::string arg1;
  std::string arg2;
  std::string dest;
};

using ExprMap = absl::flat_hash_map<std::string, Operation>;

absl::StatusOr<int> EvaluateMemo(
    absl::flat_hash_map<std::string_view, int>* memo,
    const ExprMap& ops_by_dest, std::string_view dest) {
  if (auto it = memo->find(dest); it != memo->end()) return it->second;

  int ret;
  if (!absl::SimpleAtoi(dest, &ret)) {
    auto it = ops_by_dest.find(dest);
    if (it == ops_by_dest.end()) {
      return Error("No ", dest);
    }
    const Operation& op = it->second;
    if (op.operation == "") {
      ASSIGN_OR_RETURN(int in, EvaluateMemo(memo, ops_by_dest, op.arg1));
      ret = in;
    } else if (op.operation == "NOT") {
      ASSIGN_OR_RETURN(int in, EvaluateMemo(memo, ops_by_dest, op.arg1));
      ret = ~in;
    } else if (op.operation == "AND") {
      ASSIGN_OR_RETURN(int in1, EvaluateMemo(memo, ops_by_dest, op.arg1));
      ASSIGN_OR_RETURN(int in2, EvaluateMemo(memo, ops_by_dest, op.arg2));
      ret = in1 & in2;
    } else if (op.operation == "OR") {
      ASSIGN_OR_RETURN(int in1, EvaluateMemo(memo, ops_by_dest, op.arg1));
      ASSIGN_OR_RETURN(int in2, EvaluateMemo(memo, ops_by_dest, op.arg2));
      ret = in1 | in2;
    } else if (op.operation == "LSHIFT") {
      ASSIGN_OR_RETURN(int in1, EvaluateMemo(memo, ops_by_dest, op.arg1));
      ASSIGN_OR_RETURN(int in2, EvaluateMemo(memo, ops_by_dest, op.arg2));
      ret = in1 << in2;
    } else if (op.operation == "RSHIFT") {
      ASSIGN_OR_RETURN(int in1, EvaluateMemo(memo, ops_by_dest, op.arg1));
      ASSIGN_OR_RETURN(int in2, EvaluateMemo(memo, ops_by_dest, op.arg2));
      ret = in1 >> in2;
    }
  }

  (*memo)[dest] = ret;
  return ret;
}

absl::StatusOr<int> Evaluate(const ExprMap& ops_by_dest,
                             std::string_view dest) {
  absl::flat_hash_map<std::string_view, int> memo;
  return EvaluateMemo(&memo, ops_by_dest, dest);
}

absl::StatusOr<ExprMap> Parse(absl::Span<std::string_view> input) {
  ExprMap ops_by_dest;

  for (std::string_view str : input) {
    const auto [op_str, dest] = PairSplit(str, " -> ");
    Operation op;
    op.dest = dest;
    if (RE2::FullMatch(op_str, "(\\d+|[a-z]+)", &op.arg1)) {
      op.operation = "";
    } else if (RE2::FullMatch(op_str, "(\\d+|[a-z]+)", &op.arg1)) {
      op.operation = "";
    } else if (RE2::FullMatch(op_str, "NOT (\\d+|[a-z]+)", &op.arg1)) {
      op.operation = "NOT";
    } else if (RE2::FullMatch(op_str, "(\\d+|[a-z]+) AND (\\d+|[a-z]+)",
                              &op.arg1, &op.arg2)) {
      op.operation = "AND";
    } else if (RE2::FullMatch(op_str, "(\\d+|[a-z]+) OR (\\d+|[a-z]+)",
                              &op.arg1, &op.arg2)) {
      op.operation = "OR";
    } else if (RE2::FullMatch(op_str, "(\\d+|[a-z]+) LSHIFT (\\d+|[a-z]+)",
                              &op.arg1, &op.arg2)) {
      op.operation = "LSHIFT";
    } else if (RE2::FullMatch(op_str, "(\\d+|[a-z]+) RSHIFT (\\d+|[a-z]+)",
                              &op.arg1, &op.arg2)) {
      op.operation = "RSHIFT";
    } else {
      return Error("Bad op: ", op_str);
    }
    ops_by_dest[op.dest] = op;
  }

  return ops_by_dest;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_07::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ExprMap ops_by_dest, Parse(input));
  return AdventReturn(Evaluate(ops_by_dest, "a"));
}

absl::StatusOr<std::string> Day_2015_07::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ExprMap ops_by_dest, Parse(input));
  ASSIGN_OR_RETURN(int a_val, Evaluate(ops_by_dest, "a"));
  ops_by_dest["b"] = Operation{.arg1 = absl::StrCat(a_val)};
  return AdventReturn(Evaluate(ops_by_dest, "a"));
}

}  // namespace advent_of_code

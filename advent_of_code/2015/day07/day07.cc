// https://adventofcode.com/2015/day/7
//
// --- Day 7: Some Assembly Required ---
// -------------------------------------
//
// This year, Santa brought little Bobby Tables a set of wires and
// bitwise logic gates! Unfortunately, little Bobby is a little under the
// recommended age range, and he needs help assembling the circuit.
//
// Each wire has an identifier (some lowercase letters) and can carry a
// 16-bit signal (a number from 0 to 65535). A signal is provided to each
// wire by a gate, another wire, or some specific value. Each wire can
// only get a signal from one source, but can provide its signal to
// multiple destinations. A gate provides no signal until all of its
// inputs have a signal.
//
// The included instructions booklet describes how to connect the parts
// together: x AND y -> z means to connect wires x and y to an AND gate,
// and then connect its output to wire z.
//
// For example:
//
// * 123 -> x means that the signal 123 is provided to wire x.
//
// * x AND y -> z means that the bitwise AND of wire x and wire y is
// provided to wire z.
//
// * p LSHIFT 2 -> q means that the value from wire p is left-shifted
// by 2 and then provided to wire q.
//
// * NOT e -> f means that the bitwise complement of the value from
// wire e is provided to wire f.
//
// Other possible gates include OR (bitwise OR) and RSHIFT (right-shift).
// If, for some reason, you'd like to emulate the circuit instead, almost
// all programming languages (for example, C, JavaScript, or Python)
// provide operators for these gates.
//
// For example, here is a simple circuit:
//
// 123 -> x
// 456 -> y
// x AND y -> d
// x OR y -> e
// x LSHIFT 2 -> f
// y RSHIFT 2 -> g
// NOT x -> h
// NOT y -> i
//
// After it is run, these are the signals on the wires:
//
// d: 72
// e: 507
// f: 492
// g: 114
// h: 65412
// i: 65079
// x: 123
// y: 456
//
// In little Bobby's kit's instructions booklet (provided as your puzzle
// input), what signal is ultimately provided to wire a?
//
// --- Part Two ---
// ----------------
//
// Now, take the signal you got on wire a, override wire b to that
// signal, and reset the other wires (including wire a). What new signal
// is ultimately provided to wire a?

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

absl::StatusOr<int> EvaluateMemo(
    absl::flat_hash_map<absl::string_view, int>* memo,
    const absl::flat_hash_map<std::string, Operation>& ops_by_dest,
    absl::string_view dest) {
  if (auto it = memo->find(dest); it != memo->end()) return it->second;

  int ret;
  if (!absl::SimpleAtoi(dest, &ret)) {
    auto it = ops_by_dest.find(dest);
    if (it == ops_by_dest.end()) {
      return Error("No ", dest);
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
    absl::Span<absl::string_view> input) {
  absl::flat_hash_map<std::string, Operation> ops_by_dest;

  for (absl::string_view str : input) {
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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, Operation>> ops_by_dest =
      Parse(input);
  if (!ops_by_dest.ok()) return ops_by_dest.status();
  return IntReturn(Evaluate(*ops_by_dest, "a"));
}

absl::StatusOr<std::string> Day_2015_07::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, Operation>> ops_by_dest =
      Parse(input);
  if (!ops_by_dest.ok()) return ops_by_dest.status();
  absl::StatusOr<int> a_val = Evaluate(*ops_by_dest, "a");
  if (!a_val.ok()) return a_val.status();

  (*ops_by_dest)["b"] = Operation{.arg1 = absl::StrCat(*a_val)};
  return IntReturn(Evaluate(*ops_by_dest, "a"));
}

}  // namespace advent_of_code

#include "advent_of_code/2018/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2018/vm.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

std::vector<int> ApplyOpcode(VM::OpCode op_code, std::vector<int> args,
                             std::vector<int> registers) {
  CHECK_EQ(args.size(), 4);
  CHECK_EQ(registers.size(), 4);
  VM::Op op = {op_code, args[1], args[2], args[3]};
  op.Apply(registers);
  return registers;
}

absl::flat_hash_set<VM::OpCode> CheckAllOpcodes(
    std::vector<int> registers_in, std::vector<int> op,
    std::vector<int> registers_out) {
  CHECK_EQ(registers_in.size(), 4);
  CHECK_EQ(registers_out.size(), 4);
  absl::flat_hash_set<VM::OpCode> possible;
  for (int i = 1; i <= 16; ++i) {
    VM::OpCode op_code = static_cast<VM::OpCode>(i);
    std::vector<int> calc = ApplyOpcode(op_code, op, registers_in);
    VLOG(2) << i << ": {" << absl::StrJoin(op, ",") << "} + {"
            << absl::StrJoin(registers_in, ",") << "} = {"
            << absl::StrJoin(calc, ",") << "} (test: {"
            << absl::StrJoin(registers_out, ",") << "}";
    if (registers_out == calc) possible.insert(op_code);
  }
  return possible;
}

absl::StatusOr<absl::flat_hash_map<int, VM::OpCode>> ComputeMap(
    absl::flat_hash_map<int, absl::flat_hash_set<VM::OpCode>> possible) {
  absl::flat_hash_map<int, VM::OpCode> map;
  absl::flat_hash_map<VM::OpCode, int> reverse;
  while (!possible.empty()) {
    std::vector<VM::OpCode> assigned;
    for (const auto& [op_int, set] : possible) {
      if (set.size() == 1) {
        VM::OpCode assign = *set.begin();
        if (map.contains(op_int)) return Error("Dupe in map");
        if (reverse.contains(assign)) return Error("Dupe in reverse");
        assigned.push_back(assign);
        map[op_int] = assign;
        reverse[assign] = op_int;
      }
    }
    if (assigned.empty()) return Error("Can't determine map");
    std::vector<int> to_clear;
    for (VM::OpCode op : assigned) {
      for (auto& [op_int, set] : possible) {
        set.erase(op);
        if (set.size() == 0) to_clear.push_back(op_int);
      }
    }
    for (int op_int : to_clear) possible.erase(op_int);
  }
  return map;
}

}  // namespace

absl::StatusOr<std::string> Day_2018_16::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<int> register_in(4, 0);
  std::vector<int> op(4, 0);
  std::vector<int> register_out(4, 0);
  int count = 0;
  int empty_count = 0;
  for (absl::string_view row : input) {
    if (!row.empty()) empty_count = 0;
    if (RE2::FullMatch(row, "Before: \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]",
                       &register_in[0], &register_in[1], &register_in[2],
                       &register_in[3])) {
      // OK.
    } else if (RE2::FullMatch(row, "(\\d+) (\\d+) (\\d+) (\\d+)", &op[0],
                              &op[1], &op[2], &op[3])) {
      // OK.
    } else if (RE2::FullMatch(row,
                              "After:  \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]",
                              &register_out[0], &register_out[1],
                              &register_out[2], &register_out[3])) {
      if (CheckAllOpcodes(register_in, op, register_out).size() >= 3) ++count;
    } else if (row.empty()) {
      ++empty_count;
      if (empty_count == 3) break;
    } else {
      return Error("Can't parse line: ", row);
    }
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2018_16::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<int> register_in(4, 0);
  std::vector<int> op(4, 0);
  std::vector<int> register_out(4, 0);
  int empty_count = 0;
  absl::flat_hash_map<int, absl::flat_hash_set<VM::OpCode>> possible;
  absl::flat_hash_map<int, VM::OpCode> map;
  for (absl::string_view row : input) {
    if (!row.empty()) empty_count = 0;
    if (RE2::FullMatch(row, "Before: \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]",
                       &register_in[0], &register_in[1], &register_in[2],
                       &register_in[3])) {
      if (!map.empty()) return Error("'Before' line after building map");
    } else if (RE2::FullMatch(row, "(\\d+) (\\d+) (\\d+) (\\d+)", &op[0],
                              &op[1], &op[2], &op[3])) {
      if (!map.empty()) {
        if (!map.contains(op[0])) return Error("Bad op: ", op[0]);
        register_in = ApplyOpcode(map[op[0]], op, register_in);
      } else {
        // OK.
      }
    } else if (RE2::FullMatch(row,
                              "After:  \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]",
                              &register_out[0], &register_out[1],
                              &register_out[2], &register_out[3])) {
      if (!map.empty()) return Error("'After' line after building map");
      absl::flat_hash_set<VM::OpCode> this_possible =
          CheckAllOpcodes(register_in, op, register_out);
      if (!possible.contains(op[0])) {
        possible[op[0]].insert(this_possible.begin(), this_possible.end());
      } else {
        std::vector<VM::OpCode> to_delete;
        for (VM::OpCode test_op : possible[op[0]]) {
          if (!this_possible.contains(test_op)) {
            to_delete.push_back(test_op);
          }
        }
        for (VM::OpCode del : to_delete) {
          possible[op[0]].erase(del);
        }
      }
    } else if (row.empty()) {
      if (!map.empty()) return Error("'Empty' line after building map");
      ++empty_count;
      if (empty_count == 3) {
        absl::StatusOr<absl::flat_hash_map<int, VM::OpCode>> computed_map =
            ComputeMap(possible);
        if (!computed_map.ok()) return computed_map.status();
        map = std::move(*computed_map);
        if (map.size() != 16) return Error("Missing opcodes");
        register_in = {0, 0, 0, 0};
      }
    } else {
      return Error("Can't parse line: ", row);
    }
  }
  return IntReturn(register_in[0]);
}

}  // namespace advent_of_code

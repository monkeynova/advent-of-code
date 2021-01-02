#include "advent_of_code/2018/day16/day16.h"

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

enum OpCode {
  kAddr = 1,
  kAddi = 2,
  kMulr = 3,
  kMuli = 4,
  kBanr = 5,
  kBani = 6,
  kBorr = 7,
  kBori = 8,
  kSetr = 9,
  kSeti = 10,
  kGtir = 11,
  kGtri = 12,
  kGtrr = 13,
  kEqir = 14,
  kEqri = 15,
  kEqrr = 16,
};

std::vector<int> ApplyOpcode(OpCode op_code, std::vector<int> op, std::vector<int> registers) {
  CHECK_EQ(op.size(), 4);
  CHECK_EQ(registers.size(), 4);
  switch (op_code) {
    case OpCode::kAddr: {
      registers[op[3]] = registers[op[1]] + registers[op[2]];
      break;
    }
    case OpCode::kAddi: {
      registers[op[3]] = registers[op[1]] + op[2];
      break;
    }
    case OpCode::kMulr: {
      registers[op[3]] = registers[op[1]] * registers[op[2]];
      break;
    }
    case OpCode::kMuli: {
      registers[op[3]] = registers[op[1]] * op[2];
      break;
    }
    case OpCode::kBanr: {
      registers[op[3]] = registers[op[1]] & registers[op[2]];
      break;
    }
    case OpCode::kBani: {
      registers[op[3]] = registers[op[1]] & op[2];
      break;
    }
    case OpCode::kBorr: {
      registers[op[3]] = registers[op[1]] | registers[op[2]];
      break;
    }
    case OpCode::kBori: {
      registers[op[3]] = registers[op[1]] | op[2];
      break;
    }
    case OpCode::kSetr: {
      registers[op[3]] = registers[op[1]];
      break;
    }
    case OpCode::kSeti : {
      registers[op[3]] = op[1];
      break;
    }
    case OpCode::kGtir : {
      registers[op[3]] = op[1] > registers[op[2]] ? 1 : 0;
      break;
    }
    case OpCode::kGtri : {
      registers[op[3]] = registers[op[1]] > op[2] ? 1 : 0;
      break;
    }
    case OpCode::kGtrr : {
      registers[op[3]] = registers[op[1]] > registers[op[2]] ? 1 : 0;
      break;
    }
    case OpCode::kEqir : {
      registers[op[3]] = op[1] == registers[op[2]] ? 1 : 0;
      break;
    }
    case OpCode::kEqri : {
      registers[op[3]] = registers[op[1]] == op[2] ? 1 : 0;
      break;
    }
    case OpCode::kEqrr : {
      registers[op[3]] = registers[op[1]] == registers[op[2]] ? 1 : 0;
      break;
    }
  }
  return registers;
}

absl::flat_hash_set<OpCode> CheckAllOpcodes(std::vector<int> registers_in, std::vector<int> op, std::vector<int> registers_out) {
  CHECK_EQ(registers_in.size(), 4);
  CHECK_EQ(registers_out.size(), 4);
  absl::flat_hash_set<OpCode> possible;
  for (int i = 1; i <= 16; ++i) {
    OpCode op_code = static_cast<OpCode>(i);
    std::vector<int> calc = ApplyOpcode(op_code, op, registers_in);
    VLOG(2) << i << ": {" << absl::StrJoin(op, ",") << "} + {"
            << absl::StrJoin(registers_in, ",") << "} = {"
            << absl::StrJoin(calc, ",") << "} (test: {"
            << absl::StrJoin(registers_out, ",") << "}";
    if (registers_out == calc) possible.insert(op_code);
  }
  return possible;
}

absl::StatusOr<absl::flat_hash_map<int, OpCode>> ComputeMap(
    absl::flat_hash_map<int, absl::flat_hash_set<OpCode>> possible) {
  absl::flat_hash_map<int, OpCode> map;
  absl::flat_hash_map<OpCode, int> reverse;
  while (!possible.empty()) {
    std::vector<OpCode> assigned;
    for (const auto& [op_int, set] : possible) {
      if (set.size() == 1) {
        OpCode assign = *set.begin();
        if (map.contains(op_int)) return AdventDay::Error("Dupe in map");
        if (reverse.contains(assign)) return AdventDay::Error("Dupe in reverse");
        assigned.push_back(assign);
        map[op_int] = assign;
        reverse[assign] = op_int;
      }
    }
    if (assigned.empty()) return AdventDay::Error("Can't determine map");
    std::vector<int> to_clear;
    for (OpCode op : assigned) {
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

absl::StatusOr<std::vector<std::string>> Day16_2018::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<int> register_in(4, 0);
  std::vector<int> op(4, 0);
  std::vector<int> register_out(4, 0);
  int count = 0;
  int empty_count = 0;
  for (absl::string_view row : input) {
    if (!row.empty()) empty_count = 0;
    if (RE2::FullMatch(row, "Before: \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]",
                             &register_in[0], &register_in[1], &register_in[2], &register_in[3])) {
      // OK.
    } else if (RE2::FullMatch(row, "(\\d+) (\\d+) (\\d+) (\\d+)",
                              &op[0], &op[1], &op[2], &op[3])) {
      // OK.
    } else if (RE2::FullMatch(row, "After:  \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]",
                             &register_out[0], &register_out[1], &register_out[2], &register_out[3])) {
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

absl::StatusOr<std::vector<std::string>> Day16_2018::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<int> register_in(4, 0);
  std::vector<int> op(4, 0);
  std::vector<int> register_out(4, 0);
  int empty_count = 0;
  absl::flat_hash_map<int, absl::flat_hash_set<OpCode>> possible;
  absl::flat_hash_map<int, OpCode> map;
  for (absl::string_view row : input) {
    if (!row.empty()) empty_count = 0;
    if (RE2::FullMatch(row, "Before: \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]",
                             &register_in[0], &register_in[1], &register_in[2], &register_in[3])) {
      if (!map.empty()) return Error("'Before' line after building map");
    } else if (RE2::FullMatch(row, "(\\d+) (\\d+) (\\d+) (\\d+)",
                              &op[0], &op[1], &op[2], &op[3])) {
      if (!map.empty()) {
        if (!map.contains(op[0])) return Error("Bad op: ", op[0]);
        register_in = ApplyOpcode(map[op[0]], op, register_in);
      } else {
        // OK.
      }
    } else if (RE2::FullMatch(row, "After:  \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]",
                             &register_out[0], &register_out[1], &register_out[2], &register_out[3])) {
      if (!map.empty()) return Error("'After' line after building map");
      absl::flat_hash_set<OpCode> this_possible = CheckAllOpcodes(register_in, op, register_out);
      if (!possible.contains(op[0])) {
        possible[op[0]].insert(this_possible.begin(), this_possible.end());
      } else {
        std::vector<OpCode> to_delete;
        for (OpCode test_op : possible[op[0]]) {
          if (!this_possible.contains(test_op)) {
            to_delete.push_back(test_op);
          }
        }
        for (OpCode del : to_delete) {
          possible[op[0]].erase(del);
        }
      }
    } else if (row.empty()) {
      if (!map.empty()) return Error("'Empty' line after building map");
      ++empty_count;
      if (empty_count == 3) {
        absl::StatusOr<absl::flat_hash_map<int, OpCode>> computed_map = ComputeMap(possible);
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

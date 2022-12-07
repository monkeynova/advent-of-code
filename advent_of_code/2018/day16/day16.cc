// https://adventofcode.com/2018/day/16
//
// --- Day 16: Chronal Classification ---
// --------------------------------------
//
// As you see the Elves defend their hot chocolate successfully, you go
// back to falling through time. This is going to become a problem.
//
// If you're ever going to return to your own time, you need to
// understand how this device on your wrist works. You have a little
// while before you reach your next destination, and with a bit of trial
// and error, you manage to pull up a programming manual on the device's
// tiny screen.
//
// According to the manual, the device has four registers (numbered 0
// through 3) that can be manipulated by instructions containing one of
// 16 opcodes. The registers start with the value 0.
//
// Every instruction consists of four values: an opcode, two inputs
// (named A and B), and an output (named C), in that order. The opcode
// specifies the behavior of the instruction and how the inputs are
// interpreted. The output, C, is always treated as a register.
//
// In the opcode descriptions below, if something says "value A", it
// means to take the number given as A literally. (This is also called an
// "immediate" value.) If something says "register A", it means to use
// the number given as A to read from (or write to) the register with
// that number. So, if the opcode addi adds register A and value B,
// storing the result in register C, and the instruction addi 0 7 3 is
// encountered, it would add 7 to the value contained by register 0 and
// store the sum in register 3, never modifying registers 0, 1, or 2 in
// the process.
//
// Many opcodes are similar except for how they interpret their
// arguments. The opcodes fall into seven general categories:
//
// Addition:
//
// * addr (add register) stores into register C the result of adding
// register A and register B.
//
// * addi (add immediate) stores into register C the result of adding
// register A and value B.
//
// Multiplication:
//
// * mulr (multiply register) stores into register C the result of
// multiplying register A and register B.
//
// * muli (multiply immediate) stores into register C the result of
// multiplying register A and value B.
//
// Bitwise AND:
//
// * banr (bitwise AND register) stores into register C the result of
// the bitwise AND of register A and register B.
//
// * bani (bitwise AND immediate) stores into register C the result of
// the bitwise AND of register A and value B.
//
// Bitwise OR:
//
// * borr (bitwise OR register) stores into register C the result of
// the bitwise OR of register A and register B.
//
// * bori (bitwise OR immediate) stores into register C the result of
// the bitwise OR of register A and value B.
//
// Assignment:
//
// * setr (set register) copies the contents of register A into
// register C. (Input B is ignored.)
//
// * seti (set immediate) stores value A into register C. (Input B is
// ignored.)
//
// Greater-than testing:
//
// * gtir (greater-than immediate/register) sets register C to 1 if
// value A is greater than register B. Otherwise, register C is set
// to 0.
//
// * gtri (greater-than register/immediate) sets register C to 1 if
// register A is greater than value B. Otherwise, register C is set
// to 0.
//
// * gtrr (greater-than register/register) sets register C to 1 if
// register A is greater than register B. Otherwise, register C is
// set to 0.
//
// Equality testing:
//
// * eqir (equal immediate/register) sets register C to 1 if value A is
// equal to register B. Otherwise, register C is set to 0.
//
// * eqri (equal register/immediate) sets register C to 1 if register A
// is equal to value B. Otherwise, register C is set to 0.
//
// * eqrr (equal register/register) sets register C to 1 if register A
// is equal to register B. Otherwise, register C is set to 0.
//
// Unfortunately, while the manual gives the name of each opcode, it
// doesn't seem to indicate the number. However, you can monitor the CPU
// to see the contents of the registers before and after instructions are
// executed to try to work them out. Each opcode has a number from 0
// through 15, but the manual doesn't say which is which. For example,
// suppose you capture the following sample:
//
// Before: [3, 2, 1, 1]
// 9 2 1 2
// After:  [3, 2, 2, 1]
//
// This sample shows the effect of the instruction 9 2 1 2 on the
// registers. Before the instruction is executed, register 0 has value 3,
// register 1 has value 2, and registers 2 and 3 have value 1. After the
// instruction is executed, register 2's value becomes 2.
//
// The instruction itself, 9 2 1 2, means that opcode 9 was executed with
// A=2, B=1, and C=2. Opcode 9 could be any of the 16 opcodes listed
// above, but only three of them behave in a way that would cause the
// result shown in the sample:
//
// * Opcode 9 could be mulr: register 2 (which has a value of 1) times
// register 1 (which has a value of 2) produces 2, which matches the
// value stored in the output register, register 2.
//
// * Opcode 9 could be addi: register 2 (which has a value of 1) plus
// value 1 produces 2, which matches the value stored in the output
// register, register 2.
//
// * Opcode 9 could be seti: value 2 matches the value stored in the
// output register, register 2; the number given for B is irrelevant.
//
// None of the other opcodes produce the result captured in the sample.
// Because of this, the sample above behaves like three opcodes.
//
// You collect many of these samples (the first section of your puzzle
// input). The manual also includes a small test program (the second
// section of your puzzle input) - you can ignore it for now.
//
// Ignoring the opcode numbers, how many samples in your puzzle input
// behave like three or more opcodes?
//
// --- Part Two ---
// ----------------
//
// Using the samples you collected, work out the number of each opcode
// and execute the test program (the second section of your puzzle
// input).
//
// What value is contained in register 0 after executing the test
// program?

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

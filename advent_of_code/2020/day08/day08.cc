// https://adventofcode.com/2020/day/8
//
// --- Day 8: Handheld Halting ---
// -------------------------------
//
// Your flight to the major airline hub reaches cruising altitude without
// incident. While you consider checking the in-flight menu for one of
// those drinks that come with a little umbrella, you are interrupted by
// the kid sitting next to you.
//
// Their handheld game console won't turn on! They ask if you can take a
// look.
//
// You narrow the problem down to a strange infinite loop in the boot
// code (your puzzle input) of the device. You should be able to fix it,
// but first you need to be able to run the code in isolation.
//
// The boot code is represented as a text file with one instruction per
// line of text. Each instruction consists of an operation (acc, jmp, or
// nop) and an argument (a signed number like +4 or -20).
//
// * acc increases or decreases a single global value called the
// accumulator by the value given in the argument. For example, acc
// +7 would increase the accumulator by 7. The accumulator starts at
// 0. After an acc instruction, the instruction immediately below it
// is executed next.
//
// * jmp jumps to a new instruction relative to itself. The next
// instruction to execute is found using the argument as an offset
// from the jmp instruction; for example, jmp +2 would skip the next
// instruction, jmp +1 would continue to the instruction immediately
// below it, and jmp -20 would cause the instruction 20 lines above
// to be executed next.
//
// * nop stands for No OPeration - it does nothing. The instruction
// immediately below it is executed next.
//
// For example, consider the following program:
//
// nop +0
// acc +1
// jmp +4
// acc +3
// jmp -3
// acc -99
// acc +1
// jmp -4
// acc +6
//
// These instructions are visited in this order:
//
// nop +0  | 1
// acc +1  | 2, 8(!)
// jmp +4  | 3
// acc +3  | 6
// jmp -3  | 7
// acc -99 |
// acc +1  | 4
// jmp -4  | 5
// acc +6  |
//
// First, the nop +0 does nothing. Then, the accumulator is increased
// from 0 to 1 (acc +1) and jmp +4 sets the next instruction to the other
// acc +1 near the bottom. After it increases the accumulator from 1 to
// 2, jmp -4 executes, setting the next instruction to the only acc +3.
// It sets the accumulator to 5, and jmp -3 causes the program to
// continue back at the first acc +1.
//
// This is an infinite loop: with this sequence of jumps, the program
// will run forever. The moment the program tries to run any instruction
// a second time, you know it will never terminate.
//
// Immediately before the program would run an instruction a second time,
// the value in the accumulator is 5.
//
// Run your copy of the boot code. Immediately before any instruction is
// executed a second time, what value is in the accumulator?
//
// --- Part Two ---
// ----------------
//
// After some careful analysis, you believe that exactly one instruction
// is corrupted.
//
// Somewhere in the program, either a jmp is supposed to be a nop, or a
// nop is supposed to be a jmp. (No acc instructions were harmed in the
// corruption of this boot code.)
//
// The program is supposed to terminate by attempting to execute an
// instruction immediately after the last instruction in the file. By
// changing exactly one jmp or nop, you can repair the boot code and make
// it terminate correctly.
//
// For example, consider the same program from above:
//
// nop +0
// acc +1
// jmp +4
// acc +3
// jmp -3
// acc -99
// acc +1
// jmp -4
// acc +6
//
// If you change the first instruction from nop +0 to jmp +0, it would
// create a single-instruction infinite loop, never leaving that
// instruction. If you change almost any of the jmp instructions, the
// program will still eventually find another jmp instruction and loop
// forever.
//
// However, if you change the second-to-last instruction (from jmp -4 to
// nop -4), the program terminates! The instructions are visited in this
// order:
//
// nop +0  | 1
// acc +1  | 2
// jmp +4  | 3
// acc +3  |
// jmp -3  |
// acc -99 |
// acc +1  | 4   nop    -4  | 5
// acc +6  | 6
//
// After the last instruction (acc +6), the program terminates by
// attempting to run the instruction below the last instruction in the
// file. With this change, after the program terminates, the accumulator
// contains the value 8 (acc +1, acc +1, acc +6).
//
// Fix the program so that it terminates normally by changing exactly one
// jmp (to nop) or nop (to jmp). What is the value of the accumulator
// after the program terminates?

#include "advent_of_code/2020/day08/day08.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

class BootCode {
 public:
  enum class Instruction { kJmp = 0, kAcc = 1, kNop = 2 };
  struct Statement {
    Instruction i;
    int64_t arg;
  };

  BootCode(std::vector<Statement> statements)
      : statements_(std::move(statements)) {}

  static absl::StatusOr<BootCode> Parse(absl::Span<absl::string_view> input);

  absl::StatusOr<bool> Execute();

  int accumulator() { return accumulator_; }
  const std::vector<Statement>& statements() { return statements_; }

 private:
  std::vector<Statement> statements_;
  int accumulator_ = 0;
};

// static
absl::StatusOr<BootCode> BootCode::Parse(absl::Span<absl::string_view> input) {
  absl::flat_hash_map<absl::string_view, Instruction> imap = {
      {"jmp", Instruction::kJmp},
      {"acc", Instruction::kAcc},
      {"nop", Instruction::kNop}};
  std::vector<Statement> statements;
  for (absl::string_view str : input) {
    absl::string_view cmd;
    int arg;
    static LazyRE2 statement_re{"([a-z]*) ([\\-+]?\\d+)"};
    if (!RE2::FullMatch(str, *statement_re, &cmd, &arg)) {
      return absl::InvalidArgumentError(absl::StrCat("re: ", str));
    }
    auto it = imap.find(cmd);
    if (it == imap.end()) return absl::InvalidArgumentError("cmd");
    statements.push_back(Statement{it->second, arg});
  }
  return BootCode(std::move(statements));
}

absl::StatusOr<bool> BootCode::Execute() {
  absl::flat_hash_set<int> hist;
  int ip = 0;
  while (!hist.contains(ip)) {
    hist.insert(ip);
    if (ip >= statements_.size()) return true;
    switch (statements_[ip].i) {
      case Instruction::kJmp:
        ip += statements_[ip].arg;
        break;
      case Instruction::kAcc:
        accumulator_ += statements_[ip].arg;
        ++ip;
        break;
      case Instruction::kNop:
        ++ip;
        break;
      default:
        return absl::InternalError("bad instruction");
    }
  }
  return false;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_08::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<BootCode> boot_code = BootCode::Parse(input);
  if (!boot_code.ok()) return boot_code.status();

  absl::StatusOr<bool> terminated = boot_code->Execute();
  if (!terminated.ok()) return terminated.status();

  return IntReturn(boot_code->accumulator());
}

absl::StatusOr<std::string> Day_2020_08::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<BootCode> boot_code = BootCode::Parse(input);
  if (!boot_code.ok()) return boot_code.status();

  for (int i = 0; i < boot_code->statements().size(); ++i) {
    std::vector<BootCode::Statement> edit_statements = boot_code->statements();
    if (edit_statements[i].i == BootCode::Instruction::kJmp) {
      edit_statements[i].i = BootCode::Instruction::kNop;
    } else if (edit_statements[i].i == BootCode::Instruction::kNop) {
      edit_statements[i].i = BootCode::Instruction::kJmp;
    } else {
      continue;
    }
    BootCode tmp_code(std::move(edit_statements));
    absl::StatusOr<bool> terminated = tmp_code.Execute();
    if (!terminated.ok()) return terminated.status();
    if (*terminated) {
      return IntReturn(tmp_code.accumulator());
    }
  }

  return absl::InvalidArgumentError("No version terminated");
}

}  // namespace advent_of_code

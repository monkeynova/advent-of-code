// https://adventofcode.com/2015/day/23
//
// --- Day 23: Opening the Turing Lock ---
// ---------------------------------------
// 
// Little Jane Marie just got her very first computer for Christmas from
// some unknown benefactor. It comes with instructions and an example
// program, but the computer itself seems to be malfunctioning. She's
// curious what the program does, and would like you to help her run it.
// 
// The manual explains that the computer supports two registers and six
// instructions (truly, it goes on to remind the reader, a
// state-of-the-art technology). The registers are named a and b, can
// hold any non-negative integer, and begin with a value of 0. The
// instructions are as follows:
// 
// * hlf r sets register r to half its current value, then continues
// with the next instruction.
// 
// * tpl r sets register r to triple its current value, then continues
// with the next instruction.
// 
// * inc r increments register r, adding 1 to it, then continues with
// the next instruction.
// 
// * jmp offset is a jump; it continues with the instruction offset
// away relative to itself.
// 
// * jie r, offset is like jmp, but only jumps if register r is even
// ("jump if even").
// 
// * jio r, offset is like jmp, but only jumps if register r is 1
// ("jump if one", not odd).
// 
// All three jump instructions work with an offset relative to that
// instruction. The offset is always written with a prefix + or - to
// indicate the direction of the jump (forward or backward,
// respectively). For example, jmp +1 would simply continue with the next
// instruction, while jmp +0 would continuously jump back to itself
// forever.
// 
// The program exits when it tries to run an instruction beyond the ones
// defined.
// 
// For example, this program sets a to 2, because the jio instruction
// causes it to skip the tpl instruction:
// 
// inc a
// jio a, +2
// tpl a
// inc a
// 
// What is the value in register b when the program in your puzzle input
// is finished executing?
//
// --- Part Two ---
// ----------------
// 
// The unknown benefactor is very thankful for releasi-- er, helping
// little Jane Marie with her computer. Definitely not to distract you,
// what is the value in register b after the program is finished
// executing if register a starts as 1 instead?


#include "advent_of_code/2015/day23/day23.h"

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

class VM {
 public:
  static absl::StatusOr<VM> Parse(absl::Span<absl::string_view> input) {
    VM ret;
    for (absl::string_view ins_str : input) {
      Instruction ins;
      if (RE2::FullMatch(ins_str, "hlf ([ab])", &ins.reg)) {
        ins.op = OpCode::kHlf;
      } else if (RE2::FullMatch(ins_str, "tpl ([ab])", &ins.reg)) {
        ins.op = OpCode::kTpl;
      } else if (RE2::FullMatch(ins_str, "inc ([ab])", &ins.reg)) {
        ins.op = OpCode::kInc;
      } else if (RE2::FullMatch(ins_str, "jmp ([+-]\\d+)", &ins.literal)) {
        ins.op = OpCode::kJmp;
      } else if (RE2::FullMatch(ins_str, "jie ([ab]), ([+-]\\d+)", &ins.reg,
                                &ins.literal)) {
        ins.op = OpCode::kJie;
      } else if (RE2::FullMatch(ins_str, "jio ([ab]), ([+-]\\d+)", &ins.reg,
                                &ins.literal)) {
        ins.op = OpCode::kJio;
      } else {
        return Error("Bad instruction: ", ins_str);
      }
      ret.instructions_.push_back(ins);
    }
    return ret;
  }

  void Execute() {
    int ip = 0;
    while (ip < instructions_.size()) {
      const Instruction& ins = instructions_[ip];
      int64_t* reg = ins.reg.empty()
                         ? nullptr
                         : ins.reg == "a" ? &register_a_ : &register_b_;
      switch (ins.op) {
        case OpCode::kHlf: {
          *reg /= 2;
          ++ip;
          break;
        }
        case OpCode::kTpl: {
          *reg *= 3;
          ++ip;
          break;
        }
        case OpCode::kInc: {
          (*reg)++;
          ++ip;
          break;
        }
        case OpCode::kJmp: {
          ip += ins.literal;
          break;
        }
        case OpCode::kJie: {
          if (*reg % 2 == 0) {
            ip += ins.literal;
          } else {
            ++ip;
          }
          break;
        }
        case OpCode::kJio: {
          if (*reg == 1) {
            ip += ins.literal;
          } else {
            ++ip;
          }
          break;
        }
      }
    }
  }

  int64_t set_register_a(int64_t v) { return register_a_ = v; }

  int64_t register_b() const { return register_b_; }

 private:
  enum class OpCode {
    kHlf = 1,
    kTpl = 2,
    kInc = 3,
    kJmp = 4,
    kJie = 5,
    kJio = 6,
  };
  struct Instruction {
    OpCode op;
    absl::string_view reg;
    int64_t literal;
  };

  VM() = default;

  int64_t register_a_ = 0;
  int64_t register_b_ = 0;
  std::vector<Instruction> instructions_;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2015_23::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();
  vm->Execute();
  return IntReturn(vm->register_b());
}

absl::StatusOr<std::string> Day_2015_23::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();
  vm->set_register_a(1);
  vm->Execute();
  return IntReturn(vm->register_b());
}

}  // namespace advent_of_code

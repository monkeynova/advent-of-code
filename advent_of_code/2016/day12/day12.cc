#include "advent_of_code/2016/day12/day12.h"

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

enum class OpCode {
  kCpy = 1,
  kInc = 2,
  kDec = 3,
  kJnz = 4,
};

struct Instruction {
  OpCode op_code;
  absl::string_view arg1;
  absl::string_view arg2;

  static absl::StatusOr<Instruction> Parse(absl::string_view in) {
    Instruction i;
    if (RE2::FullMatch(in, "cpy ([-a-z0-9]+) ([a-z]+)", &i.arg1, &i.arg2)) {
      i.op_code = OpCode::kCpy;
    } else if (RE2::FullMatch(in, "inc ([a-z]+)", &i.arg1)) {
      i.op_code = OpCode::kInc;
    } else if (RE2::FullMatch(in, "dec ([a-z]+)", &i.arg1)) {
      i.op_code = OpCode::kDec;
    } else if (RE2::FullMatch(in, "jnz ([-a-z0-9]+) ([-a-z0-9]+)", &i.arg1,
                              &i.arg2)) {
      i.op_code = OpCode::kJnz;
    } else {
      return AdventDay::Error("Bad instruction: ", in);
    }
    return i;
  }
};

struct Registers {
  int64_t a = 0;
  int64_t b = 0;
  int64_t c = 0;
  int64_t d = 0;
  int64_t* Val(absl::string_view name) {
    static int64_t literal;
    if (name == "a") return &a;
    if (name == "b") return &b;
    if (name == "c") return &c;
    if (name == "d") return &d;
    if (absl::SimpleAtoi(name, &literal)) return &literal;
    return nullptr;
  }
};

class VM {
 public:
  static absl::StatusOr<VM> Parse(absl::Span<absl::string_view> input) {
    VM ret;
    for (absl::string_view in : input) {
      absl::StatusOr<Instruction> i = Instruction::Parse(in);
      if (!i.ok()) return i.status();
      ret.instructions_.push_back(std::move(*i));
    }
    return ret;
  }

  absl::Status Execute() {
    int ip = 0;
    while (ip < instructions_.size()) {
      if (ip < 0) return AdventDay::Error("Bad ip: ", ip);
      const Instruction& i = instructions_[ip];
      switch (i.op_code) {
        case OpCode::kCpy: {
          int64_t* input = registers_.Val(i.arg1);
          if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          int64_t* output = registers_.Val(i.arg2);
          if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg2);
          *output = *input;
          ++ip;
          break;
        }
        case OpCode::kInc: {
          int64_t* output = registers_.Val(i.arg1);
          if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          ++*output;
          ++ip;
          break;
        }
        case OpCode::kDec: {
          int64_t* output = registers_.Val(i.arg1);
          if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          --*output;
          ++ip;
          break;
        }
        case OpCode::kJnz: {
          int64_t* input = registers_.Val(i.arg1);
          if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          int64_t* dest = registers_.Val(i.arg2);
          if (dest == nullptr) return AdventDay::Error("Bad arg: ", i.arg2);
          if (*input != 0) {
            ip += *dest;
          } else {
            ++ip;
          }
          break;
        }
      }
    }
    return absl::OkStatus();
  }

  int64_t register_a() { return registers_.a; }
  void set_register_c(int64_t v) { registers_.c = v; }

 private:
  VM() = default;

  std::vector<Instruction> instructions_;
  Registers registers_;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day12_2016::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();

  if (absl::Status st = vm->Execute(); !st.ok()) return st;

  return IntReturn(vm->register_a());
}

absl::StatusOr<std::vector<std::string>> Day12_2016::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();

  vm->set_register_c(1);
  if (absl::Status st = vm->Execute(); !st.ok()) return st;

  return IntReturn(vm->register_a());
}

}  // namespace advent_of_code

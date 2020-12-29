#include "advent_of_code/2017/day18/day18.h"

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

struct Instruction {
  enum {
    kSnd = 1,
    kSet = 2,
    kAdd = 3,
    kMul = 4,
    kMod = 5,
    kRcv = 6,
    kJgz = 7,
  } op_code;
  absl::string_view arg1;
  absl::string_view arg2;

  static absl::StatusOr<Instruction> Parse(absl::string_view str) {
    Instruction ret;
    if (RE2::FullMatch(str, "snd (-?\\d+|[a-z])", &ret.arg1)) {
      ret.op_code = Instruction::kSnd;
    } else if (RE2::FullMatch(str, "set ([a-z]) (-?\\d+|[a-z])", &ret.arg1, &ret.arg2)) {
      ret.op_code = Instruction::kSet;
    } else if (RE2::FullMatch(str, "add ([a-z]) (-?\\d+|[a-z])", &ret.arg1, &ret.arg2)) {
      ret.op_code = Instruction::kAdd;
    } else if (RE2::FullMatch(str, "mul ([a-z]) (-?\\d+|[a-z])", &ret.arg1, &ret.arg2)) {
      ret.op_code = Instruction::kMul;
    } else if (RE2::FullMatch(str, "mod ([a-z]) (-?\\d+|[a-z])", &ret.arg1, &ret.arg2)) {
      ret.op_code = Instruction::kMod;
    } else if (RE2::FullMatch(str, "rcv (-?\\d+|[a-z])", &ret.arg1)) {
      ret.op_code = Instruction::kRcv;
    } else if (RE2::FullMatch(str, "jgz (-?\\d+|[a-z]) (-?\\d+|[a-z])", &ret.arg1, &ret.arg2)) {
      ret.op_code = Instruction::kJgz;
    } else {
      return AdventDay::Error("Bad instruction: ", str);
    }
    return ret;
  }
};

class VM {
 public:
  static absl::StatusOr<VM> Parse(absl::Span<absl::string_view> input) {
    VM ret;
    for (absl::string_view ins : input) {
      absl::StatusOr<Instruction> next = Instruction::Parse(ins);
      if (!next.ok()) return next.status();
      ret.instructions_.push_back(*next);
    }
    return ret;
  }

  void Execute() {
    int ip = 0;
    bool done = false;
    absl::flat_hash_map<std::string_view, int64_t> registers;
    absl::string_view reg_names = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < reg_names.size(); ++i) {
      registers[reg_names.substr(i, 1)] = 0;
    }
    while (!done && ip < instructions_.size() && ip >= 0) {
      bool jumped = false;
      const Instruction& i = instructions_[ip];
      switch (i.op_code) {
        case Instruction::kSnd: {
          last_snd_ = GetValue(i.arg1, registers);
          VLOG(1) << "SND: " << last_snd_;
          break;
        }
        case Instruction::kSet: {
          registers[i.arg1] = GetValue(i.arg2, registers);
          VLOG(1) << "SET: " << i.arg1 << "=" << registers[i.arg1];
          break;
        }
        case Instruction::kAdd: {
          registers[i.arg1] += GetValue(i.arg2, registers);
          VLOG(1) << "ADD: " << i.arg1 << "+=" << GetValue(i.arg2, registers) << "=" << registers[i.arg1];
          break;
        }
        case Instruction::kMul: {
          registers[i.arg1] *= GetValue(i.arg2, registers);
          VLOG(1) << "MUL: " << i.arg1 << "*=" << GetValue(i.arg2, registers) << "=" << registers[i.arg1];
          break;
        }
        case Instruction::kMod: {
          registers[i.arg1] %= GetValue(i.arg2, registers);
          VLOG(1) << "MOD: " << i.arg1 << "*%" << GetValue(i.arg2, registers) << "=" << registers[i.arg1];
          break;
        }
        case Instruction::kRcv: {
          VLOG(1) << "RCV: " << GetValue(i.arg1, registers);
          if (GetValue(i.arg1, registers) != 0) {
            last_recv_ = last_snd_;
            done = true;
          }
          break;
        }
        case Instruction::kJgz: {
          VLOG(1) << "JGZ: " << GetValue(i.arg1, registers) << "," << GetValue(i.arg2, registers);
          if (GetValue(i.arg1, registers) > 0) {
            ip += GetValue(i.arg2, registers);
            jumped = true;
          }
          break;
        }
      }
      if (!jumped) ++ip;
    }
  }
 
  int last_recv() const { return last_recv_; }

 private:
  int GetValue(absl::string_view name,
               const absl::flat_hash_map<std::string_view, int64_t> registers) {
    if (auto it = registers.find(name); it != registers.end()) return it->second;
    int n;
    CHECK(absl::SimpleAtoi(name, &n));
    return n;
  }

  std::vector<Instruction> instructions_;
  int last_recv_ = -1;
  int last_snd_ = -1;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day18_2017::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();
  vm->Execute();
  return IntReturn(vm->last_recv());
}

absl::StatusOr<std::vector<std::string>> Day18_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

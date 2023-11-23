#include "advent_of_code/2017/vm.h"

#include "absl/strings/str_cat.h"
#include "advent_of_code/vlog.h"

namespace advent_of_code {

// static
absl::StatusOr<VM_2017::Instruction> VM_2017::Instruction::Parse(
    std::string_view str) {
  Instruction ret;
  if (RE2::FullMatch(str, "snd (-?\\d+|[a-z])", &ret.arg1)) {
    ret.op_code = Instruction::kSnd;
  } else if (RE2::FullMatch(str, "set ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                            &ret.arg2)) {
    ret.op_code = Instruction::kSet;
  } else if (RE2::FullMatch(str, "sub ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                            &ret.arg2)) {
    ret.op_code = Instruction::kSub;
  } else if (RE2::FullMatch(str, "add ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                            &ret.arg2)) {
    ret.op_code = Instruction::kAdd;
  } else if (RE2::FullMatch(str, "mul ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                            &ret.arg2)) {
    ret.op_code = Instruction::kMul;
  } else if (RE2::FullMatch(str, "mod ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                            &ret.arg2)) {
    ret.op_code = Instruction::kMod;
  } else if (RE2::FullMatch(str, "rcv (-?\\d+|[a-z])", &ret.arg1)) {
    ret.op_code = Instruction::kRcv;
  } else if (RE2::FullMatch(str, "jgz (-?\\d+|[a-z]) (-?\\d+|[a-z])", &ret.arg1,
                            &ret.arg2)) {
    ret.op_code = Instruction::kJgz;
  } else if (RE2::FullMatch(str, "jnz (-?\\d+|[a-z]) (-?\\d+|[a-z])", &ret.arg1,
                            &ret.arg2)) {
    ret.op_code = Instruction::kJnz;
  } else {
    return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
  }
  return ret;
}

// static
absl::StatusOr<VM_2017> VM_2017::Parse(absl::Span<std::string_view> input) {
  VM_2017 ret;
  for (std::string_view ins : input) {
    ASSIGN_OR_RETURN(Instruction next, Instruction::Parse(ins));
    ret.instructions_.push_back(next);
  }
  return ret;
}

void VM_2017::ExecuteToRecv() {
  bool recv = false;
  while (!recv && ip_ < instructions_.size() && ip_ >= 0) {
    bool jumped = false;
    const Instruction& i = instructions_[ip_];
    switch (i.op_code) {
      case Instruction::kSnd: {
        send_queue_.push_back(GetValue(i.arg1));
        break;
      }
      case Instruction::kSet: {
        registers_[i.arg1] = GetValue(i.arg2);
        break;
      }
      case Instruction::kAdd: {
        registers_[i.arg1] += GetValue(i.arg2);
        break;
      }
      case Instruction::kSub: {
        registers_[i.arg1] -= GetValue(i.arg2);
        break;
      }
      case Instruction::kMul: {
        ++mul_count_;
        registers_[i.arg1] *= GetValue(i.arg2);
        break;
      }
      case Instruction::kMod: {
        registers_[i.arg1] %= GetValue(i.arg2);
        break;
      }
      case Instruction::kRcv: {
        if (!recv_queue_.empty()) {
          registers_[i.arg1] = recv_queue_.front();
          recv_queue_.pop_front();
        } else {
          recv = true;
          // Don't advance IP. Come back here.
          jumped = true;
        }
        break;
      }
      case Instruction::kJgz: {
        if (GetValue(i.arg1) > 0) {
          ip_ += GetValue(i.arg2);
          jumped = true;
        }
        break;
      }
      case Instruction::kJnz: {
        if (GetValue(i.arg1) != 0) {
          ip_ += GetValue(i.arg2);
          jumped = true;
        }
        break;
      }
    }
    if (!jumped) ++ip_;
  }
}

}  // namespace advent_of_code
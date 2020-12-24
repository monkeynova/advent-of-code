#include "advent_of_code/2016/assem_bunny.h"

namespace advent_of_code {

// static
absl::StatusOr<AssemBunny::Instruction> AssemBunny::Instruction::Parse(
    absl::string_view in) {
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
  } else if (RE2::FullMatch(in, "tgl ([-a-z0-9]+)", &i.arg1)) {
    i.op_code = OpCode::kTgl;
  } else if (RE2::FullMatch(in, "out ([-a-z0-9]+)", &i.arg1)) {
    i.op_code = OpCode::kOut;
  } else {
    return AdventDay::Error("Bad instruction: ", in);
  }
  return i;
}

absl::Status AssemBunny::Execute(OutputInterface* output_interface,
                                 PauseInterface* pause_interface) {
  while (ip_ < instructions_.size()) {
    if (pause_interface != nullptr && pause_interface->Pause()) break;
    if (ip_ < 0) return AdventDay::Error("Bad ip: ", ip_);
    const Instruction& i = instructions_[ip_];
    bool jumped = false;
    switch (i.op_code) {
      case OpCode::kCpy: {
        int64_t* input = registers_.Val(i.arg1);
        if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
        int64_t* output = registers_.Val(i.arg2);
        if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg2);
        *output = *input;
        break;
      }
      case OpCode::kInc: {
        int64_t* output = registers_.Val(i.arg1);
        if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
        ++*output;
        break;
      }
      case OpCode::kDec: {
        int64_t* output = registers_.Val(i.arg1);
        if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
        --*output;
        break;
      }
      case OpCode::kJnz: {
        int64_t* input = registers_.Val(i.arg1);
        if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
        int64_t* dest = registers_.Val(i.arg2);
        if (dest == nullptr) return AdventDay::Error("Bad arg: ", i.arg2);
        if (*input != 0) {
          ip_ += *dest;
          jumped = true;
        }
        break;
      }
      case OpCode::kTgl: {
        int64_t* input = registers_.Val(i.arg1);
        if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
        if (*input + ip_ >= 0 && *input + ip_ < instructions_.size()) {
          Instruction& to_edit = instructions_[*input + ip_];
          switch (to_edit.op_code) {
            // One-arg ops.
            case OpCode::kInc:
              to_edit.op_code = OpCode::kDec;
              break;
            case OpCode::kDec:
              to_edit.op_code = OpCode::kInc;
              break;
            case OpCode::kTgl:
              to_edit.op_code = OpCode::kInc;
              break;
            // Two arg ops.
            case OpCode::kJnz:
              to_edit.op_code = OpCode::kCpy;
              break;
            case OpCode::kCpy:
              to_edit.op_code = OpCode::kJnz;
              break;
            default:
              return AdventDay::Error("Bad op to toggle");
          }
        }
        break;
      }
      case OpCode::kOut: {
        int64_t* output = registers_.Val(i.arg1);
        if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
        if (output_interface == nullptr)
          return AdventDay::Error("No output sink");
        if (absl::Status st = output_interface->OnOutput(*output, this);
            !st.ok())
          return st;
        break;
      }
    }
    if (!jumped) ++ip_;
  }
  return absl::OkStatus();
}

}  // namespace advent_of_code
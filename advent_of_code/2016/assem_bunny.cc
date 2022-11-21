#include "advent_of_code/2016/assem_bunny.h"

namespace advent_of_code {

namespace {

struct AssemBunnyArg {
  AssemBunny::Registers* registers;
  std::variant<int64_t*, int64_t>* arg;

  static bool Parse(const char* raw_str, size_t n, void* dest_untyped) {
    absl::string_view str(raw_str, n);
    AssemBunnyArg* dest = static_cast<AssemBunnyArg*>(dest_untyped);

    bool ret = true;
    if (str == "a") {
      *dest->arg = &dest->registers->a;
    } else if (str == "b") {
      *dest->arg = &dest->registers->b;
    } else if (str == "c") {
      *dest->arg = &dest->registers->c;
    } else if (str == "d") {
      *dest->arg = &dest->registers->d;
    } else if (int64_t literal; absl::SimpleAtoi(str, &literal)) {
      *dest->arg = literal;
    } else {
      ret = false;
    }
    return ret;
  }

  RE2::Arg Capture() { return RE2::Arg(this, AssemBunnyArg::Parse); }
};

}  // namespace

// static
absl::StatusOr<AssemBunny::Instruction> AssemBunny::Instruction::Parse(
    absl::string_view in, AssemBunny::Registers* registers) {
  Instruction i;
  AssemBunnyArg arg1{.registers = registers, .arg = &i.arg1};
  AssemBunnyArg arg2{.registers = registers, .arg = &i.arg2};
  if (RE2::FullMatch(in, "cpy ([-a-z0-9]+) ([a-z]+)", arg1.Capture(),
                     arg2.Capture())) {
    i.op_code = OpCode::kCpy;
  } else if (RE2::FullMatch(in, "inc ([a-z]+)", arg1.Capture())) {
    i.op_code = OpCode::kInc;
  } else if (RE2::FullMatch(in, "dec ([a-z]+)", arg1.Capture())) {
    i.op_code = OpCode::kDec;
  } else if (RE2::FullMatch(in, "jnz ([-a-z0-9]+) ([-a-z0-9]+)", arg1.Capture(),
                            arg2.Capture())) {
    i.op_code = OpCode::kJnz;
  } else if (RE2::FullMatch(in, "tgl ([-a-z0-9]+)", arg1.Capture())) {
    i.op_code = OpCode::kTgl;
  } else if (RE2::FullMatch(in, "out ([-a-z0-9]+)", arg1.Capture())) {
    i.op_code = OpCode::kOut;
  } else {
    return Error("Bad instruction: ", in);
  }
  return i;
}

void AssemBunny::Instruction::RemapRegisters(Registers* from, Registers* to) {
  int64_t delta = reinterpret_cast<char*>(to) - reinterpret_cast<char*>(from);
  if (std::holds_alternative<int64_t*>(arg1)) {
    arg1 = reinterpret_cast<int64_t*>(
        reinterpret_cast<char*>(std::get<int64_t*>(arg1)) + delta);
  }
  if (std::holds_alternative<int64_t*>(arg2)) {
    arg2 = reinterpret_cast<int64_t*>(
        reinterpret_cast<char*>(std::get<int64_t*>(arg2)) + delta);
  }
}

// static
absl::StatusOr<AssemBunny> AssemBunny ::Parse(
    absl::Span<absl::string_view> input) {
  AssemBunny ret;
  for (absl::string_view in : input) {
    absl::StatusOr<Instruction> i =
        Instruction::Parse(in, ret.registers_.get());
    if (!i.ok()) return i.status();
    ret.instructions_.push_back(std::move(*i));
  }
  return ret;
}

AssemBunny AssemBunny::Clone() const {
  AssemBunny ret;
  ret.registers_ = std::make_unique<Registers>(*registers_);
  ret.ip_ = ip_;
  ret.instructions_ = instructions_;
  for (auto& i : ret.instructions_) {
    i.RemapRegisters(registers_.get(), ret.registers_.get());
  }
  return ret;
}

absl::Status AssemBunny::Execute(OutputInterface* output_interface,
                                 PauseInterface* pause_interface) {
  while (ip_ < instructions_.size()) {
    if (pause_interface != nullptr && pause_interface->Pause()) break;
    if (ip_ < 0) return Error("Bad ip: ", ip_);
    const Instruction& i = instructions_[ip_];
    bool jumped = false;
    switch (i.op_code) {
      case OpCode::kCpy: {
        int64_t input = i.Arg1();
        int64_t* output = i.MutableArg2();
        *output = input;
        break;
      }
      case OpCode::kInc: {
        int64_t* output = i.MutableArg1();
        ++*output;
        break;
      }
      case OpCode::kDec: {
        int64_t* output = i.MutableArg1();
        --*output;
        break;
      }
      case OpCode::kJnz: {
        int64_t input = i.Arg1();
        int64_t dest = i.Arg2();
        if (input != 0) {
          ip_ += dest;
          jumped = true;
        }
        break;
      }
      case OpCode::kTgl: {
        int64_t input = i.Arg1();
        if (input + ip_ >= 0 && input + ip_ < instructions_.size()) {
          Instruction& to_edit = instructions_[input + ip_];
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
              return Error("Bad op to toggle");
          }
        }
        break;
      }
      case OpCode::kOut: {
        int64_t* output = i.MutableArg1();
        if (output_interface == nullptr) return Error("No output sink");
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
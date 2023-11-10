#include "advent_of_code/2019/int_code.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/vlog.h"

namespace advent_of_code {

enum class OpCode {
  kUnknown = 0,
  kAdd = 1,
  kMul = 2,
  kInput = 3,
  kOutput = 4,
  kJNZ = 5,
  kJZ = 6,
  kLT = 7,
  kEQ = 8,
  kIncR = 9,
  kTerm = 99,
};

struct OpCodeMeta {
  OpCode code = OpCode::kUnknown;
  int size = 1;
  std::string name = "???";
};

static void SetOpCode(OpCodeMeta op, std::vector<OpCodeMeta>* dest) {
  (*dest)[static_cast<int>(op.code)] = op;
}

static const std::vector<OpCodeMeta>& CodeTypes() {
  static auto code_types = []() {
    std::vector<OpCodeMeta> ret(100);
    SetOpCode({.code = OpCode::kAdd, .size = 4, .name = "ADD"}, &ret);
    SetOpCode({.code = OpCode::kMul, .size = 4, .name = "MUL"}, &ret);
    SetOpCode({.code = OpCode::kInput, .size = 2, .name = "IN"}, &ret);
    SetOpCode({.code = OpCode::kOutput, .size = 2, .name = "OUT"}, &ret);
    SetOpCode({.code = OpCode::kJNZ, .size = 3, .name = "JNZ"}, &ret);
    SetOpCode({.code = OpCode::kJZ, .size = 3, .name = "JZ"}, &ret);
    SetOpCode({.code = OpCode::kLT, .size = 4, .name = "LT"}, &ret);
    SetOpCode({.code = OpCode::kEQ, .size = 4, .name = "EQ"}, &ret);
    SetOpCode({.code = OpCode::kIncR, .size = 2, .name = "INCR"}, &ret);
    SetOpCode({.code = OpCode::kTerm, .size = 1, .name = "TERM"}, &ret);
    return ret;
  }();

  return code_types;
};

absl::StatusOr<IntCode> IntCode::Parse(absl::Span<std::string_view> input) {
  if (input.empty()) {
    return absl::InvalidArgumentError("input is empty");
  }
  if (input.size() > 1) {
    return absl::InvalidArgumentError(
        absl::StrCat("input is to large: 1 !=", input.size()));
  }
  std::vector<std::string_view> code_strs = absl::StrSplit(input[0], ",");
  std::vector<int64_t> codes;
  codes.reserve(code_strs.size());
  for (std::string_view str : code_strs) {
    codes.push_back(0);
    if (!absl::SimpleAtoi(str, &codes.back())) {
      return absl::InvalidArgumentError(
          absl::StrCat("\"", str, "\" isn't parseable as an int64_t"));
    }
  }
  IntCode ret(codes);
  VLOG(3) << ret.DebugDisasm();
  return ret;
}

// `parameter_number` is 1-indexed to make code easier to read in accessors`.
static int64_t GetParameterMode(int64_t parameter_modes,
                                int64_t parameter_number) {
  --parameter_number;
  while (parameter_number) {
    parameter_modes /= 10;
    --parameter_number;
  }
  return parameter_modes % 10;
}

absl::StatusOr<int64_t> IntCode::LoadParameter(int64_t parameter_modes,
                                               int64_t parameter) {
  const int64_t parameter_mode = GetParameterMode(parameter_modes, parameter);
  switch (parameter_mode) {
    case 0: {
      // Address.
      const int64_t address = codes_[code_pos_ + parameter];
      if (address < 0) {
        return absl::InvalidArgumentError(
            absl::StrCat("Save to negative address"));
      }
      if (address >= codes_.size()) {
        codes_.resize(address + 1);
      }
      return codes_[address];
    }
    case 1: {
      // Immediate.
      return codes_[code_pos_ + parameter];
    }
    case 2: {
      // Relative address.
      const int64_t address = relative_base_ + codes_[code_pos_ + parameter];
      if (address < 0) {
        return absl::InvalidArgumentError(
            absl::StrCat("Save to negative address"));
      }
      if (address >= codes_.size()) {
        codes_.resize(address + 1);
      }
      return codes_[address];
    }
    default: {
      return absl::InvalidArgumentError(
          absl::StrCat("Invalid parameter mode: ", parameter_mode));
    }
  }
}

absl::Status IntCode::SaveParameter(int64_t parameter_modes, int64_t parameter,
                                    int64_t value) {
  const int64_t parameter_mode = GetParameterMode(parameter_modes, parameter);
  switch (parameter_mode) {
    case 0: {
      // Address.
      const int64_t address = codes_[code_pos_ + parameter];
      if (address < 0) {
        return absl::InvalidArgumentError(
            absl::StrCat("Save to negative address"));
      }
      if (address >= codes_.size()) {
        codes_.resize(address + 1);
      }
      codes_[address] = value;
      return absl::OkStatus();
    }
    case 1: {
      // Immediate.
      return absl::InvalidArgumentError(
          absl::StrCat("Immediate mode for output parameter"));
    }
    case 2: {
      // Relative address.
      const int64_t address = relative_base_ + codes_[code_pos_ + parameter];
      if (address < 0) {
        return absl::InvalidArgumentError(
            absl::StrCat("Save to negative address"));
      }
      if (address >= codes_.size()) {
        codes_.resize(address + 1);
      }
      codes_[address] = value;
      return absl::OkStatus();
    }
    default: {
      return absl::InvalidArgumentError(
          absl::StrCat("Invalid parameter mode: ", parameter_mode));
    }
  }
}

absl::Status IntCode::Run(InputSource* input, OutputSink* output,
                          PauseCondition* pause_condition) {
  while (!terminated_ && !pause_condition->PauseIntCode()) {
    if (absl::Status st = RunSingleOpcode(input, output); !st.ok()) return st;
  }
  return absl::OkStatus();
}

absl::Status IntCode::RunSingleOpcode(InputSource* input, OutputSink* output) {
  if (code_pos_ < 0 || code_pos_ >= codes_.size()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Attempt to read from pos=", code_pos_));
  }
  const int64_t opcode = codes_[code_pos_] % 100;
  const int64_t parameter_modes = codes_[code_pos_] / 100;
  const int code_size = CodeTypes()[opcode].size;
  if (code_pos_ + code_size > codes_.size()) {
    // Ensure we have space to read the arguments before accessing them.
    return absl::InvalidArgumentError(
        absl::StrCat("Attempt to read argument from pos=", code_pos_));
  }
  bool jumped = false;
  VLOG(4) << "@" << code_pos_ << ": " << CodeTypes()[opcode].name;
  switch (static_cast<OpCode>(opcode)) {
    case OpCode::kAdd: {
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 + *in2);
          !st.ok())
        return st;
      break;
    }
    case OpCode::kMul: {
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 * *in2);
          !st.ok())
        return st;
      break;
    }
    case OpCode::kInput: {
      if (input == nullptr) {
        return absl::InvalidArgumentError("No input specified");
      }
      absl::StatusOr<int64_t> input_val = input->Fetch();
      if (!input_val.ok()) return input_val.status();
      if (absl::Status st = SaveParameter(parameter_modes, 1, *input_val);
          !st.ok()) {
        return st;
      }
      break;
    }
    case OpCode::kOutput: {
      absl::StatusOr<int64_t> in = LoadParameter(parameter_modes, 1);
      if (!in.ok()) return in.status();
      if (output == nullptr) {
        return absl::InvalidArgumentError("No output specified");
      }
      if (absl::Status st = output->Put(*in); !st.ok()) return st;
      break;
    }
    case OpCode::kJNZ: {
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      if (*in1 != 0) {
        absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
        if (!in2.ok()) return in2.status();
        code_pos_ = *in2;
        jumped = true;
      }
      break;
    }
    case OpCode::kJZ: {
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      if (*in1 == 0) {
        absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
        if (!in2.ok()) return in2.status();
        code_pos_ = *in2;
        jumped = true;
      }
      break;
    }
    case OpCode::kLT: {
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 < *in2);
          !st.ok()) {
        return st;
      }
      break;
    }
    case OpCode::kEQ: {
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 == *in2);
          !st.ok()) {
        return st;
      }
      break;
    }
    case OpCode::kIncR: {
      absl::StatusOr<int64_t> in = LoadParameter(parameter_modes, 1);
      if (!in.ok()) return in.status();
      relative_base_ += *in;
      break;
    }
    case OpCode::kTerm: {
      terminated_ = true;
      break;
    }
    default: {
      return absl::InvalidArgumentError(absl::StrCat(
          "Invalid instruction (@", code_pos_, "): ", codes_[code_pos_]));
    }
      if (!jumped) {
        code_pos_ += CodeTypes()[opcode].size;
      }
  }

  if (!jumped) {
    code_pos_ += code_size;
  }

  return absl::OkStatus();
}

std::string IntCode::DebugDisasm() const {
  const std::vector<OpCodeMeta>& code_types = CodeTypes();
  std::string ret;
  for (int i = 0; i < codes_.size();) {
    const int64_t opcode = std::max<int>(codes_[i] % 100, 0);
    const int64_t parameter_modes = codes_[i] / 100;
    absl::StrAppend(&ret, i, ": ", code_types[opcode].name);
    for (int j = 1; j < code_types[opcode].size; ++j) {
      if (i + j >= codes_.size()) {
        absl::StrAppend(&ret, " ?");
      } else {
        int param_value = codes_[i + j];
        int param_mode = GetParameterMode(parameter_modes, j);
        switch (param_mode) {
          case 0: {
            absl::StrAppend(&ret, " A*", param_value);
            break;
          }
          case 1: {
            absl::StrAppend(&ret, " L$", param_value);
            break;
          }
          case 2: {
            absl::StrAppend(&ret, " R+", param_value);
            break;
          }
          default: {
            absl::StrAppend(&ret, " ??", param_value);
            break;
          }
        }
      }
    }
    i += code_types[opcode].size;
    absl::StrAppend(&ret, "\n");
  }
  return ret;
}

}  // namespace advent_of_code

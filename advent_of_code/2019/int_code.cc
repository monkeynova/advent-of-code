#include "advent_of_code/2019/int_code.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

absl::StatusOr<IntCode> IntCode::Parse(
    const std::vector<absl::string_view>& input) {
  if (input.size() != 1) {
    return absl::InvalidArgumentError("input is empty");
  }
  std::vector<absl::string_view> code_strs = absl::StrSplit(input[0], ",");
  std::vector<int64_t> codes;
  codes.reserve(code_strs.size());
  for (absl::string_view str : code_strs) {
    codes.push_back(0);
    if (!absl::SimpleAtoi(str, &codes.back())) {
      return absl::InvalidArgumentError(
          absl::StrCat("\"", str, "\" isn't parseable as an int64_t"));
    }
  }
  return IntCode(codes);
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

absl::StatusOr<absl::optional<int64_t>> IntCode::RunToNextOutput(
    InputSource* input) {
  std::vector<int64_t> output;
  VectorOutput output_sink(&output);
  while (!terminated_) {
    if (absl::Status st = RunSingleOpcode(input, &output_sink); !st.ok()) {
      return st;
    }
    if (!output.empty()) return output[0];
  }
  return absl::nullopt;
}

absl::Status IntCode::RunSingleOpcode(InputSource* input, OutputSink* output) {
  if (code_pos_ < 0 || code_pos_ >= codes_.size()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Attempt to read from pos=", code_pos_));
  }
  const int64_t opcode = codes_[code_pos_] % 100;
  const int64_t parameter_modes = codes_[code_pos_] / 100;
  switch (opcode) {
    VLOG(2) << "@" << code_pos_;
    case 1: {
      if (code_pos_ + 4 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 + *in2);
          !st.ok())
        return st;
      code_pos_ += 4;
      break;
    }
    case 2: {
      if (code_pos_ + 4 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 * *in2);
          !st.ok())
        return st;
      code_pos_ += 4;
      break;
    }
    case 3: {
      if (code_pos_ + 2 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      if (input == nullptr) {
        return absl::InvalidArgumentError("No input specified");
      }
      absl::StatusOr<int64_t> input_val = input->Fetch();
      if (!input_val.ok()) return input_val.status();
      if (absl::Status st = SaveParameter(parameter_modes, 1, *input_val);
          !st.ok())
        return st;
      code_pos_ += 2;
      break;
    }
    case 4: {
      if (code_pos_ + 2 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int64_t> in = LoadParameter(parameter_modes, 1);
      if (!in.ok()) return in.status();
      if (output == nullptr) {
        return absl::InvalidArgumentError("No output specified");
      }
      if (absl::Status st = output->Put(*in); !st.ok()) return st;
      code_pos_ += 2;
      break;
    }
    case 5: {
      if (code_pos_ + 3 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      if (*in1 != 0) {
        absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
        if (!in2.ok()) return in2.status();
        code_pos_ = *in2;
      } else {
        code_pos_ += 3;
      }
      break;
    }
    case 6: {
      if (code_pos_ + 3 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      if (*in1 == 0) {
        absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
        if (!in2.ok()) return in2.status();
        code_pos_ = *in2;
      } else {
        code_pos_ += 3;
      }
      break;
    }
    case 7: {
      if (code_pos_ + 4 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 < *in2);
          !st.ok())
        return st;
      code_pos_ += 4;
      break;
    }
    case 8: {
      if (code_pos_ + 4 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int64_t> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int64_t> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 == *in2);
          !st.ok())
        return st;
      code_pos_ += 4;
      break;
    }
    case 9: {
      if (code_pos_ + 2 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int64_t> in = LoadParameter(parameter_modes, 1);
      if (!in.ok()) return in.status();
      relative_base_ += *in;
      code_pos_ += 2;
      break;
    }
    case 99: {
      terminated_ = true;
      ++code_pos_;
      break;
    }
    default: {
      return absl::InvalidArgumentError(absl::StrCat(
          "Invalid instruction (@", code_pos_, "): ", codes_[code_pos_]));
    }
  }

  return absl::OkStatus();
}

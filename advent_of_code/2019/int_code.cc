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
  std::vector<int> codes;
  codes.reserve(code_strs.size());
  for (absl::string_view str : code_strs) {
    codes.push_back(0);
    if (!absl::SimpleAtoi(str, &codes.back())) {
      return absl::InvalidArgumentError(
          absl::StrCat("\"", str, "\" isn't parseable as an int"));
    }
  }
  return IntCode(codes);
}

// `parameter_number` is 1-indexed to make code easier to read in accessors`.
static int GetParameterMode(int parameter_modes, int parameter_number) {
  --parameter_number;
  while (parameter_number) {
    parameter_modes /= 10;
    --parameter_number;
  }
  return parameter_modes % 10;
}

absl::StatusOr<int> IntCode::LoadParameter(int parameter_modes, int parameter) const {
  const int parameter_mode = GetParameterMode(parameter_modes, parameter);
  switch (parameter_mode) {
    case 0: {
      // Address.
      const int address = codes_[code_pos_ + parameter];
      return codes_[address];
    }
    case 1: {
      // Immediate.
      return codes_[code_pos_ + parameter];
    }
    default: {
      return absl::InvalidArgumentError(
          absl::StrCat("Invalid parameter mode: ", parameter_mode));
    }
  }
}

absl::Status IntCode::SaveParameter(int parameter_modes, int parameter, int value) {
  const int parameter_mode = GetParameterMode(parameter_modes, parameter);
  switch (parameter_mode) {
    case 0: {
      // Address.
      const int address = codes_[code_pos_ + parameter];
      codes_[address] = value;
      return absl::OkStatus();
    }
    case 1: {
      // Immediate.
      return absl::InvalidArgumentError(
          absl::StrCat("Immediate mode for output parameter"));
    }
    default: {
      return absl::InvalidArgumentError(
          absl::StrCat("Invalid parameter mode: ", parameter_mode));
    }
  }
}

absl::Status IntCode::Run(const std::vector<int>& input, std::vector<int>* output) {
  // Run program.
  int input_pos = 0;
  while (!terminated_) {
    if (absl::Status st = RunSingleOpcode(input, input_pos, output); !st.ok()) return st;
  }
  return absl::OkStatus();
}

absl::Status IntCode::RunSingleOpcode(const std::vector<int>& input, int& input_pos, std::vector<int>* output) {
  if (code_pos_ < 0 || code_pos_ >= codes_.size()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Attempt to read from pos=", code_pos_));
  }
  const int opcode = codes_[code_pos_] % 100;
  const int parameter_modes = codes_[code_pos_] / 100;
  switch (opcode) {
    VLOG(2) << "@" << code_pos_;
    case 1: {
      if (code_pos_ + 4 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 + *in2); !st.ok()) return st;
      code_pos_ += 4;
      break;
    }
    case 2: {
      if (code_pos_ + 4 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 * *in2); !st.ok()) return st;
      code_pos_ += 4;
      break;
    }
    case 3: {
      if (code_pos_ + 2 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      if (input_pos >= input.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read past end of input"));
      }
      if (absl::Status st = SaveParameter(parameter_modes, 1, input[input_pos]); !st.ok()) return st;
      ++input_pos;
      code_pos_ += 2;
      break;
    }
    case 4: {
      if (code_pos_ + 2 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int> in = LoadParameter(parameter_modes, 1);
      if (!in.ok()) return in.status();
      if (output == nullptr) {
        return absl::InvalidArgumentError("Nowhere to output");
      }
      output->push_back(*in);
      code_pos_ += 2;
      break;
    }
    case 5: {
      if (code_pos_ + 3 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      if (*in1 != 0) {
        absl::StatusOr<int> in2 = LoadParameter(parameter_modes, 2);
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
      absl::StatusOr<int> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      if (*in1 == 0) {
        absl::StatusOr<int> in2 = LoadParameter(parameter_modes, 2);
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
      absl::StatusOr<int> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 < *in2); !st.ok()) return st;
      code_pos_ += 4;
      break;
    }
    case 8: {
      if (code_pos_ + 4 >= codes_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Attempt to read from pos=", code_pos_));
      }
      absl::StatusOr<int> in1 = LoadParameter(parameter_modes, 1);
      if (!in1.ok()) return in1.status();
      absl::StatusOr<int> in2 = LoadParameter(parameter_modes, 2);
      if (!in2.ok()) return in2.status();
      if (absl::Status st = SaveParameter(parameter_modes, 3, *in1 == *in2); !st.ok()) return st;
      code_pos_ += 4;
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

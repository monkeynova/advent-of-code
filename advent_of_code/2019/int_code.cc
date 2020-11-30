#include "advent_of_code/2019/int_code.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

absl::StatusOr<std::vector<int>> ParseIntcode(
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
  return codes;
}

// `parameter_number` is 1-indexed to make code easier to read in accessors`.
int GetParameterMode(int parameter_modes, int parameter_number) {
  --parameter_number;
  while (parameter_number) {
    parameter_modes /= 10;
    --parameter_number;
  }
  return parameter_modes % 10;
}

absl::StatusOr<int> LoadParameter(const std::vector<int>* codes, int code_pos, int parameter_modes, int parameter) {
  const int parameter_mode = GetParameterMode(parameter_modes, parameter);
  switch (parameter_mode) {
    case 0: {
      // Address.
      const int address = (*codes)[code_pos + parameter];
      return (*codes)[address];
    }
    case 1: {
      // Immediate.
      return (*codes)[code_pos + parameter];
    }
    default: {
      return absl::InvalidArgumentError(
          absl::StrCat("Invalid parameter mode: ", parameter_mode));
    }
  }
}

absl::Status SaveParameter(std::vector<int>* codes, int code_pos, int parameter_modes, int parameter, int value) {
  const int parameter_mode = GetParameterMode(parameter_modes, parameter);
  switch (parameter_mode) {
    case 0: {
      // Address.
      const int address = (*codes)[code_pos + parameter];
      (*codes)[address] = value;
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

absl::Status RunIntcode(std::vector<int>* codes, const std::vector<int>& input, std::vector<int>* output) {
  // Run program.
  int code_pos = 0;
  int input_pos = 0;
  while (true) {
    if (code_pos < 0 || code_pos >= codes->size()) {
      return absl::InvalidArgumentError(
          absl::StrCat("Attempt to read from pos=", code_pos));
    }
    const int opcode = (*codes)[code_pos] % 100;
    const int parameter_modes = (*codes)[code_pos] / 100;
    switch (opcode) {
      VLOG(2) << "@" << code_pos;
      case 1: {
        if (code_pos + 4 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        absl::StatusOr<int> in1 = LoadParameter(codes, code_pos, parameter_modes, 1);
        if (!in1.ok()) return in1.status();
        absl::StatusOr<int> in2 = LoadParameter(codes, code_pos, parameter_modes, 2);
        if (!in2.ok()) return in2.status();
        if (absl::Status st = SaveParameter(codes, code_pos, parameter_modes, 3, *in1 + *in2); !st.ok()) return st;
        code_pos += 4;
        break;
      }
      case 2: {
        if (code_pos + 4 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        absl::StatusOr<int> in1 = LoadParameter(codes, code_pos, parameter_modes, 1);
        if (!in1.ok()) return in1.status();
        absl::StatusOr<int> in2 = LoadParameter(codes, code_pos, parameter_modes, 2);
        if (!in2.ok()) return in2.status();
        if (absl::Status st = SaveParameter(codes, code_pos, parameter_modes, 3, *in1 * *in2); !st.ok()) return st;
        code_pos += 4;
        break;
      }
      case 3: {
        if (code_pos + 2 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        if (input_pos >= input.size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read past end of input"));
        }
        if (absl::Status st = SaveParameter(codes, code_pos, parameter_modes, 1, input[input_pos]); !st.ok()) return st;
        ++input_pos;
        code_pos += 2;
        break;
      }
      case 4: {
        if (code_pos + 2 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        absl::StatusOr<int> in = LoadParameter(codes, code_pos, parameter_modes, 1);
        if (!in.ok()) return in.status();
        if (output == nullptr) {
          return absl::InvalidArgumentError("Nowhere to output");
        }
        output->push_back(*in);
        code_pos += 2;
        break;
      }
      case 5: {
        if (code_pos + 3 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        absl::StatusOr<int> in1 = LoadParameter(codes, code_pos, parameter_modes, 1);
        if (!in1.ok()) return in1.status();
        if (*in1 != 0) {
          absl::StatusOr<int> in2 = LoadParameter(codes, code_pos, parameter_modes, 2);
          if (!in2.ok()) return in2.status();
          code_pos = *in2;
        } else {
          code_pos += 3;
        }
        break;
      }
      case 6: {
        if (code_pos + 3 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        absl::StatusOr<int> in1 = LoadParameter(codes, code_pos, parameter_modes, 1);
        if (!in1.ok()) return in1.status();
        if (*in1 == 0) {
          absl::StatusOr<int> in2 = LoadParameter(codes, code_pos, parameter_modes, 2);
          if (!in2.ok()) return in2.status();
          code_pos = *in2;
        } else {
          code_pos += 3;
        }
        break;
      }
      case 7: {
        if (code_pos + 4 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        absl::StatusOr<int> in1 = LoadParameter(codes, code_pos, parameter_modes, 1);
        if (!in1.ok()) return in1.status();
        absl::StatusOr<int> in2 = LoadParameter(codes, code_pos, parameter_modes, 2);
        if (!in2.ok()) return in2.status();
        if (absl::Status st = SaveParameter(codes, code_pos, parameter_modes, 3, *in1 < *in2); !st.ok()) return st;
        code_pos += 4;
        break;
      }
      case 8: {
        if (code_pos + 4 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        absl::StatusOr<int> in1 = LoadParameter(codes, code_pos, parameter_modes, 1);
        if (!in1.ok()) return in1.status();
        absl::StatusOr<int> in2 = LoadParameter(codes, code_pos, parameter_modes, 2);
        if (!in2.ok()) return in2.status();
        if (absl::Status st = SaveParameter(codes, code_pos, parameter_modes, 3, *in1 == *in2); !st.ok()) return st;
        code_pos += 4;
        break;
      }
      case 99: {
        ++code_pos;
        return absl::OkStatus();
      }
      default: {
        return absl::InvalidArgumentError(absl::StrCat(
            "Invalid instruction (@", code_pos, "): ", (*codes)[code_pos]));
      }
    }
  }
}

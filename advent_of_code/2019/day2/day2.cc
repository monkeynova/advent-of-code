#include "advent_of_code/2019/day2/day2.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

absl::StatusOr<std::vector<int>> ParseIntcode(
    const std::vector<absl::string_view>& input) {
  if (input.empty()) {
    return absl::InvalidArgumentError("input is empty");
  }
  std::vector<absl::string_view> code_strs = absl::StrSplit(input[0], ",");
  if (input.size() > 1) {
    for (int i = 1; i < input.size(); ++i) {
      if (!input[i].empty()) {
        return absl::InvalidArgumentError(
            absl::StrCat("input contains second non-empty line: ", input[i]));
      }
    }
  }
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

void Part1CorrectBoard(std::vector<int>* codes) {
  (*codes)[1] = 12;
  (*codes)[2] = 2;
}

absl::Status RunIntcode(std::vector<int>* codes) {
  // Run program.
  int code_pos = 0;
  while (true) {
    if (code_pos < 0 || code_pos >= codes->size()) {
      return absl::InvalidArgumentError(
          absl::StrCat("Attempt to read from pos=", code_pos));
    }
    switch ((*codes)[code_pos]) {
      LOG(INFO) << "@" << code_pos;
      case 1: {
        if (code_pos + 4 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        int in1_pos = (*codes)[code_pos + 1];
        int in2_pos = (*codes)[code_pos + 2];
        int out_pos = (*codes)[code_pos + 3];
        LOG(INFO) << "Add(" << in1_pos << ", " << in2_pos << ") => " << out_pos;
        (*codes)[out_pos] = (*codes)[in1_pos] + (*codes)[in2_pos];
        break;
      }
      case 2: {
        if (code_pos + 4 >= codes->size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Attempt to read from pos=", code_pos));
        }
        int in1_pos = (*codes)[code_pos + 1];
        int in2_pos = (*codes)[code_pos + 2];
        int out_pos = (*codes)[code_pos + 3];
        LOG(INFO) << "Mult(" << in1_pos << ", " << in2_pos << ") => "
                  << out_pos;
        (*codes)[out_pos] = (*codes)[in1_pos] * (*codes)[in2_pos];
        break;
      }
      case 99: {
        LOG(INFO) << "Term";
        return absl::OkStatus();
      }
      default: {
        return absl::InvalidArgumentError(absl::StrCat(
            "Invalid instruction (@", code_pos, "): ", (*codes)[code_pos]));
      }
    }
    code_pos += 4;
  }
}

absl::StatusOr<std::vector<std::string>> Day2_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int>> codes = ParseIntcode(input);
  if (!codes.ok()) return codes.status();

  Part1CorrectBoard(&*codes);
  if (absl::Status st = RunIntcode(&*codes); !st.ok()) return st;

  return std::vector<std::string>{absl::StrCat((*codes)[0])};
}

absl::StatusOr<std::vector<std::string>> Day2_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}

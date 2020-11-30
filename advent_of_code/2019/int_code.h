#ifndef ADVENT_OF_CODE_2019_INT_CODE_H
#define ADVENT_OF_CODE_2019_INT_CODE_H

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

absl::StatusOr<std::vector<int>> ParseIntcode(
    const std::vector<absl::string_view>& input);

int GetParameterMode(int parameter_modes, int parameter_number);

absl::StatusOr<int> LoadParameter(const std::vector<int>* codes, int code_pos, int parameter_modes, int parameter);
absl::Status SaveParameter(std::vector<int>* codes, int code_pos, int parameter_modes, int parameter, int value);

absl::Status RunIntcode(std::vector<int>* codes, const std::vector<int>& input = {}, std::vector<int>* output = nullptr);

#endif  // ADVENT_OF_CODE_2019_INT_CODE_H
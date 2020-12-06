#ifndef ADVENT_OF_CODE_ADVENT_DAY_H
#define ADVENT_OF_CODE_ADVENT_DAY_H

#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"

class AdventDay {
 public:
  virtual ~AdventDay() = default;

  std::vector<std::string> IntReturn(int64_t val) const {
    return std::vector<std::string>{absl::StrCat(val)};
  }

  virtual absl::StatusOr<std::vector<std::string>> Part1(
      const std::vector<absl::string_view>& input) const = 0;
  virtual absl::StatusOr<std::vector<std::string>> Part2(
      const std::vector<absl::string_view>& input) const = 0;
};

#endif  // ADVENT_OF_CODE_ADVENT_DAY_H
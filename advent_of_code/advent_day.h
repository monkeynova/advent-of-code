#ifndef ADVENT_OF_CODE_ADVENT_DAY_H
#define ADVENT_OF_CODE_ADVENT_DAY_H

#include <string>
#include <vector>

#include "absl/status/statusor.h"

class AdventDay {
 public:
  virtual ~AdventDay() = default;

  virtual absl::StatusOr<std::vector<std::string>> Part1(
      const std::vector<absl::string_view>& input) = 0;
  virtual absl::StatusOr<std::vector<std::string>> Part2(
      const std::vector<absl::string_view>& input) = 0;
};

#endif  // ADVENT_OF_CODE_ADVENT_DAY_H
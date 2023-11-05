#ifndef ADVENT_OF_CODE_2023_DAY01_H
#define ADVENT_OF_CODE_2023_DAY01_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class Day_2023_01 : public AdventDay {
 public:
  virtual absl::string_view test_file() const override {
    return "advent_of_code/2023/day01/day01.test";
  }

  absl::StatusOr<std::string> Part1(
      absl::Span<absl::string_view> input) const override;
  absl::StatusOr<std::string> Part2(
      absl::Span<absl::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2023_DAY01_H
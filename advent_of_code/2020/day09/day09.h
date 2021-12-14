#ifndef ADVENT_OF_CODE_2020_DAY09_H
#define ADVENT_OF_CODE_2020_DAY09_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class Day_2020_09 : public AdventDay {
 public:
  virtual absl::string_view test_file() override {
    return "advent_of_code/2020/day09/day09.test";
  }

  absl::StatusOr<std::string> Part1(
      absl::Span<absl::string_view> input) const override;
  absl::StatusOr<std::string> Part2(
      absl::Span<absl::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2020_DAY09_H
#ifndef ADVENT_OF_CODE_2016_DAY16_H
#define ADVENT_OF_CODE_2016_DAY16_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class Day_2016_16 : public AdventDay {
 public:
  virtual absl::string_view test_file() const override {
    return "advent_of_code/2016/day16/day16.test";
  }

  absl::StatusOr<std::string> Part1(
      absl::Span<absl::string_view> input) const override;
  absl::StatusOr<std::string> Part2(
      absl::Span<absl::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2016_DAY16_H
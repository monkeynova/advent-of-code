#ifndef ADVENT_OF_CODE_2017_DAY12_H
#define ADVENT_OF_CODE_2017_DAY12_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class Day_2017_12 : public AdventDay {
 public:
  virtual std::string_view test_file() const override {
    return "advent_of_code/2017/day12/day12.test";
  }

  absl::StatusOr<std::string> Part1(
      absl::Span<std::string_view> input) const override;
  absl::StatusOr<std::string> Part2(
      absl::Span<std::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2017_DAY12_H
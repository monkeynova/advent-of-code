#ifndef ADVENT_OF_CODE_2018_DAY24_H
#define ADVENT_OF_CODE_2018_DAY24_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class Day_2018_24 : public AdventDay {
 public:
  virtual std::string_view title() const override {
    return "Day 24: Immune System Simulator 20XX";
  }

  virtual std::string_view test_file() const override {
    return "advent_of_code/2018/day24/day24.test";
  }

  absl::StatusOr<std::string> Part1(
      absl::Span<std::string_view> input) const override;
  absl::StatusOr<std::string> Part2(
      absl::Span<std::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2018_DAY24_H
#ifndef ADVENT_OF_CODE_2024_DAY16_H
#define ADVENT_OF_CODE_2024_DAY16_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class Day_2024_16 : public AdventDay {
 public:
  virtual std::string_view title() const override {
    return "Day 16: Reindeer Maze";
  }

  virtual std::string_view test_file() const override {
    return "advent_of_code/2024/day16/day16.test";
  }

  absl::StatusOr<std::string> Part1(
      absl::Span<std::string_view> input) const override;
  absl::StatusOr<std::string> Part2(
      absl::Span<std::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2024_DAY16_H
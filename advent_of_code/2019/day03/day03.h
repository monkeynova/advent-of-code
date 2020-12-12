#ifndef ADVENT_OF_CODE_2019_DAY03_H
#define ADVENT_OF_CODE_2019_DAY03_H

#include "advent_of_code/advent_day.h"

class Day03_2019 : public AdventDay {
 public:
  absl::StatusOr<std::vector<std::string>> Part1(
      absl::Span<absl::string_view> input) const override;
  absl::StatusOr<std::vector<std::string>> Part2(
      absl::Span<absl::string_view> input) const override;
};

#endif  // ADVENT_OF_CODE_2019_DAY03_H
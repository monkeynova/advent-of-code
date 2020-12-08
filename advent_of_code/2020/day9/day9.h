#ifndef ADVENT_OF_CODE_2020_DAY9_H
#define ADVENT_OF_CODE_2020_DAY9_H

#include "advent_of_code/advent_day.h"

class Day9_2020 : public AdventDay {
 public:
  absl::StatusOr<std::vector<std::string>> Part1(
      const std::vector<absl::string_view>& input) const override;
  absl::StatusOr<std::vector<std::string>> Part2(
      const std::vector<absl::string_view>& input) const override;
};

#endif  // ADVENT_OF_CODE_2020_DAY9_H
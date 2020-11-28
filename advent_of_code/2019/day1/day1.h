#ifndef ADVENT_OF_CODE_2019_DAY1_H
#define ADVENT_OF_CODE_2019_DAY1_H

#include "advent_of_code/advent_day.h"

class Day1_2019 : AdventDay {
 public:
  absl::StatusOr<std::vector<std::string>> Part1(const std::vector<absl::string_view>& input) override;
  absl::StatusOr<std::vector<std::string>> Part2(const std::vector<absl::string_view>& input) override;
};

#endif  // ADVENT_OF_CODE_2019_DAY1_H
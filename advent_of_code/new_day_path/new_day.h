#ifndef ADVENT_OF_CODE_NEW_DAY_H
#define ADVENT_OF_CODE_NEW_DAY_H

#include "advent_of_code/advent_day.h"

class NewDay : public AdventDay {
 public:
  absl::StatusOr<std::vector<std::string>> Part1(
      absl::Span<absl::string_view> input) const override;
  absl::StatusOr<std::vector<std::string>> Part2(
      absl::Span<absl::string_view> input) const override;
};

#endif  // ADVENT_OF_CODE_NEW_DAY_H
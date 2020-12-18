#ifndef ADVENT_OF_CODE_2020_DAY02_H
#define ADVENT_OF_CODE_2020_DAY02_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class Day02_2020 : public AdventDay {
 public:
  absl::StatusOr<std::vector<std::string>> Part1(
      absl::Span<absl::string_view> input) const override;
  absl::StatusOr<std::vector<std::string>> Part2(
      absl::Span<absl::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2020_DAY02_H
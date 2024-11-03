#ifndef ADVENT_OF_CODE_NEW_DAY_H
#define ADVENT_OF_CODE_NEW_DAY_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class NewDay : public AdventDay {
 public:
  virtual std::string_view title() const override { return "New Day: ???"; }

  virtual std::string_view test_file() const override {
    return "advent_of_code/new_day_path/new_day.test";
  }

  absl::StatusOr<std::string> Part1(
      absl::Span<std::string_view> input) const override;
  absl::StatusOr<std::string> Part2(
      absl::Span<std::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_NEW_DAY_H
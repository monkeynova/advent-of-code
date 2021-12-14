#ifndef ADVENT_OF_CODE_NEW_DAY_H
#define ADVENT_OF_CODE_NEW_DAY_H

#include "advent_of_code/advent_day.h"

namespace advent_of_code {

class NewDay : public AdventDay {
 public:
  virtual absl::string_view test_file() override {
    return "advent_of_code/new_day_path/new_day.test";
  }

  absl::StatusOr<std::string> Part1(
      absl::Span<absl::string_view> input) const override;
  absl::StatusOr<std::string> Part2(
      absl::Span<absl::string_view> input) const override;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_NEW_DAY_H
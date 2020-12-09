#ifndef ADVENT_OF_CODE_ADVENT_DAY_H
#define ADVENT_OF_CODE_ADVENT_DAY_H

#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"

class AdventDay {
 public:
  virtual ~AdventDay() = default;

  absl::StatusOr<std::vector<int64_t>> ParseAsInts(
      const std::vector<absl::string_view>& input) const {
    std::vector<int64_t> vals;
    for (int i = 0; i < input.size(); ++i) {
      int64_t v;
      if (!absl::SimpleAtoi(input[i], &v)) {
        return absl::InvalidArgumentError("parse");
      }
      vals.push_back(v);
    }
    return vals;
  }

  absl::StatusOr<std::vector<std::string>> IntReturn(int64_t val) const {
    return std::vector<std::string>{absl::StrCat(val)};
  }

  absl::StatusOr<std::vector<std::string>> IntReturn(
      absl::StatusOr<int64_t> val) const {
    if (!val.ok()) return val.status();
    return std::vector<std::string>{absl::StrCat(*val)};
  }

  virtual absl::StatusOr<std::vector<std::string>> Part1(
      const std::vector<absl::string_view>& input) const = 0;
  virtual absl::StatusOr<std::vector<std::string>> Part2(
      const std::vector<absl::string_view>& input) const = 0;
};

#endif  // ADVENT_OF_CODE_ADVENT_DAY_H
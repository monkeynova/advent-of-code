#ifndef ADVENT_OF_CODE_ADVENT_DAY_H
#define ADVENT_OF_CODE_ADVENT_DAY_H

#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/types/span.h"

namespace advent_of_code {

// TODO(@monkeynova): Move from const std::vector<...>& to absl::Span.
//                    That would allow children to mutate as they
//                    process.
class AdventDay {
 public:
  virtual ~AdventDay() = default;

  static absl::StatusOr<std::vector<int64_t>> ParseAsInts(
      absl::Span<absl::string_view> input) {
    std::vector<int64_t> vals;
    for (int i = 0; i < input.size(); ++i) {
      int64_t v;
      if (!absl::SimpleAtoi(input[i], &v)) {
        return Error("parse as int: ", input[i]);
      }
      vals.push_back(v);
    }
    return vals;
  }

  template <class... Args>
  static absl::Status Error(Args... args) {
    return absl::InvalidArgumentError(absl::StrCat(args...));
  }

  absl::StatusOr<std::vector<std::string>> IntReturn(int64_t val) const {
    return std::vector<std::string>{absl::StrCat(val)};
  }

  absl::StatusOr<std::vector<std::string>> IntReturn(
      absl::StatusOr<int64_t> val) const {
    if (!val.ok()) return val.status();
    return std::vector<std::string>{absl::StrCat(*val)};
  }

  absl::StatusOr<std::vector<std::string>> IntReturn(
      absl::optional<int64_t> val) const {
    if (!val) return absl::NotFoundError("Not found");
    return std::vector<std::string>{absl::StrCat(*val)};
  }

  virtual absl::StatusOr<std::vector<std::string>> Part1(
      absl::Span<absl::string_view> input) const = 0;
  virtual absl::StatusOr<std::vector<std::string>> Part2(
      absl::Span<absl::string_view> input) const = 0;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_ADVENT_DAY_H
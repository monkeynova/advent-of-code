#ifndef ADVENT_OF_CODE_ADVENT_DAY_H
#define ADVENT_OF_CODE_ADVENT_DAY_H

#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/types/span.h"

namespace advent_of_code {

class AdventDay {
 public:
  virtual ~AdventDay() = default;

  template <class Container>
  static absl::StatusOr<std::vector<int64_t>> ParseAsInts(Container input) {
    std::vector<int64_t> vals;
    for (absl::string_view in : input) {
      int64_t v;
      if (!absl::SimpleAtoi(in, &v)) {
        return Error("parse as int: ", in);
      }
      vals.push_back(v);
    }
    return vals;
  }

  template <class... Args>
  static absl::Status Error(Args... args) {
    return absl::InvalidArgumentError(absl::StrCat(args...));
  }

  absl::StatusOr<std::string> IntReturn(int64_t val) const {
    return absl::StrCat(val);
  }

  absl::StatusOr<std::string> IntReturn(absl::StatusOr<int64_t> val) const {
    if (!val.ok()) return val.status();
    return absl::StrCat(*val);
  }

  absl::StatusOr<std::string> IntReturn(absl::optional<int64_t> val) const {
    if (!val) return absl::NotFoundError("Not found");
    return absl::StrCat(*val);
  }

  virtual absl::StatusOr<std::string> Part1(
      absl::Span<absl::string_view> input) const = 0;
  virtual absl::StatusOr<std::string> Part2(
      absl::Span<absl::string_view> input) const = 0;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_ADVENT_DAY_H
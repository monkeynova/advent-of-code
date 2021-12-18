#ifndef ADVENT_OF_CODE_ADVENT_DAY_H
#define ADVENT_OF_CODE_ADVENT_DAY_H

#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/types/span.h"

extern absl::Flag<bool> FLAGS_advent_day_run_audit;

namespace advent_of_code {

template <class... Args>
inline absl::Status Error(Args... args) {
  return absl::InternalError(absl::StrCat(args...));
}

template <class Container>
inline absl::StatusOr<std::vector<int64_t>> ParseAsInts(Container input) {
  std::vector<int64_t> vals;
  for (absl::string_view in : input) {
    int64_t v;
    if (!absl::SimpleAtoi(in, &v)) {
      return absl::InvalidArgumentError(absl::StrCat("parse as int: ", in));
    }
    vals.push_back(v);
  }
  return vals;
}

inline std::pair<absl::string_view, absl::string_view> PairSplit(
    absl::string_view in, absl::string_view delim) {
  return std::pair<absl::string_view, absl::string_view>(
      absl::StrSplit(in, absl::MaxSplits(delim, 2)));
}

class AdventDay {
 public:
  virtual ~AdventDay() = default;

  bool run_audit() const { return absl::GetFlag(FLAGS_advent_day_run_audit); }

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

  virtual absl::string_view test_file() const = 0;

  virtual absl::StatusOr<std::string> Part1(
      absl::Span<absl::string_view> input) const = 0;
  virtual absl::StatusOr<std::string> Part2(
      absl::Span<absl::string_view> input) const = 0;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_ADVENT_DAY_H
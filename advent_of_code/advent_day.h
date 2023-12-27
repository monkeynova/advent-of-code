#ifndef ADVENT_OF_CODE_ADVENT_DAY_H
#define ADVENT_OF_CODE_ADVENT_DAY_H

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/log/check.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/types/span.h"
#include "advent_of_code/tokenizer.h"
#include "advent_of_code/vlog.h"

extern absl::Flag<bool> FLAGS_advent_day_run_audit;

namespace advent_of_code {

template <class... Args>
inline absl::Status Error(Args... args) {
  return absl::InternalError(absl::StrCat(std::forward<Args>(args)...));
}

template <class Container>
inline absl::StatusOr<std::vector<int64_t>> ParseAsInts(Container input) {
  std::vector<int64_t> vals;
  for (std::string_view in : input) {
    int64_t v;
    if (!absl::SimpleAtoi(in, &v)) {
      return absl::InvalidArgumentError(absl::StrCat("parse as int: ", in));
    }
    vals.push_back(v);
  }
  return vals;
}

template <typename IntType=int64_t>
inline absl::StatusOr<std::vector<IntType>> ParseAsInts(std::string_view input) {
  std::vector<IntType> vals;
  Tokenizer tok(input);
  while (!tok.Done()) {
    vals.push_back(0);
    ASSIGN_OR_RETURN(vals.back(), tok.NextInt());
  }
  return vals;
}

inline std::pair<std::string_view, std::string_view> PairSplit(
    std::string_view in, std::string_view delim) {
  return std::pair<std::string_view, std::string_view>(
      absl::StrSplit(in, absl::MaxSplits(delim, 2)));
}

class AdventDay {
 public:
  virtual ~AdventDay() = default;

  bool run_audit() const { return absl::GetFlag(FLAGS_advent_day_run_audit); }

  void set_param(std::string param) { param_ = param; }
  std::string_view param() const { return param_; }

  // Returns an integer value stored in param() or an error if parsing fails.
  absl::StatusOr<int64_t> IntParam() const;
  // IntParam1 and IntParam2 work by treating param() either as an integer or
  // as a comma separated set of two integers. If there is a single integer,
  // both functions return the same value. If there are two integers 
  // separated by a comma IntParam1 returns the first and IntParam2 the second. 
  absl::StatusOr<int64_t> IntParam1() const;
  absl::StatusOr<int64_t> IntParam2() const;

  template <typename T>
  absl::StatusOr<std::string> AdventReturn(const absl::StatusOr<T>& t) const {
    if (!t.ok()) return t.status();
    return absl::StrCat(*t);
  }

  absl::StatusOr<std::string> AdventReturn(absl::Status st) const {
    return std::move(st);
  }

  template <typename T>
  absl::StatusOr<std::string> AdventReturn(const std::optional<T>& t) const {
    if (!t) return absl::NotFoundError("Not found");
    return absl::StrCat(*t);
  }

  template <typename T>
  absl::StatusOr<std::string> AdventReturn(const T& t) const {
    return absl::StrCat(t);
  }

  virtual std::string_view test_file() const = 0;

  virtual absl::StatusOr<std::string> Part1(
      absl::Span<std::string_view> input) const = 0;
  virtual absl::StatusOr<std::string> Part2(
      absl::Span<std::string_view> input) const = 0;

 private:
  std::string param_;
};

struct AdventRegisterEntry {};

AdventRegisterEntry RegisterAdventDay(
    int year, int day, std::function<std::unique_ptr<AdventDay>()>);

std::unique_ptr<AdventDay> CreateAdventDay(int year, int day);

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_ADVENT_DAY_H
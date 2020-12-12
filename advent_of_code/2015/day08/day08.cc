#include "advent_of_code/2015/day08/day08.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day08_2015::Part1(
    absl::Span<absl::string_view> input) const {
  int delta = 0;
  for (absl::string_view str : input) {
    if (str[0] != '"') return Error("Bad input");
    if (str.back() != '"') return Error("Bad input");
    delta += 2;
    for (int i = 1; i < str.size() - 1; ++i) {
      if (str[i] == '\\') {
        if (str[i+1] == '"' || str[i+1] == '\\') {
          delta += 1;
          i += 1;
        } else {
          delta += 3;
          i += 3;
        }
      }
    }
  }
  return IntReturn(delta);
}

absl::StatusOr<std::vector<std::string>> Day08_2015::Part2(
    absl::Span<absl::string_view> input) const {
  int delta = 0;
  for (absl::string_view str : input) {
    if (str[0] != '"') return Error("Bad input");
    if (str.back() != '"') return Error("Bad input");
    delta += 4;
    for (int i = 1; i < str.size() - 1; ++i) {
      if (str[i] == '\\') {
        ++delta;
      } else if (str[i] == '"') {
        ++delta;
      }
    }
  }
  return IntReturn(delta);
}

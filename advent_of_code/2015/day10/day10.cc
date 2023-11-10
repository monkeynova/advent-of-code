#include "advent_of_code/2015/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

std::string LookAndSay(std::string_view input) {
  std::string out;
  for (int i = 0; i < input.size();) {
    int j;
    for (j = i; j < input.size() && input[j] == input[i]; ++j) {
      // Spin.
    }
    out.append(absl::StrCat(j - i, input.substr(i, 1)));
    i = j;
  }
  return out;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_10::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string str = std::string(input[0]);
  for (int i = 0; i < 40; ++i) {
    VLOG(1) << str;
    str = LookAndSay(str);
  }
  return AdventReturn(str.size());
}

absl::StatusOr<std::string> Day_2015_10::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string str = std::string(input[0]);
  for (int i = 0; i < 50; ++i) {
    VLOG(1) << str;
    str = LookAndSay(str);
  }
  return AdventReturn(str.size());
}

}  // namespace advent_of_code

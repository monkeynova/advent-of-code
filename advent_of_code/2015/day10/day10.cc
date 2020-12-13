#include "advent_of_code/2015/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

std::string LookAndSay(absl::string_view input) {
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

absl::StatusOr<std::vector<std::string>> Day10_2015::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string str = std::string(input[0]);
  for (int i = 0; i < 40; ++i) {
    VLOG(1) << str;
    str = LookAndSay(str);
  }
  return IntReturn(str.size());
}

absl::StatusOr<std::vector<std::string>> Day10_2015::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string str = std::string(input[0]);
  for (int i = 0; i < 50; ++i) {
    VLOG(1) << str;
    str = LookAndSay(str);
  }
  return IntReturn(str.size());
}

#include "advent_of_code/2016/day07/day07.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

bool SupportsTLS(absl::string_view str) {
  int level = 0;
  bool match_outside = false;
  bool match_inside = false;
  for (int i = 0; i < str.size(); ++i) {
    if (str[i] == '[') ++level;
    if (str[i] == ']') --level;
    if (i >= 3) {
      if (str[i] == str[i-3] &&
          str[i-1] == str[i-2] &&
          str[i] != str[i-1]) {
        if (level == 0) match_outside = true;
        else match_inside = true;
      }
    }
  }
  return match_outside && !match_inside;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day07_2016::Part1(
    absl::Span<absl::string_view> input) const {
  int count = 0;
  for (absl::string_view str : input) {
    bool supports = SupportsTLS(str);
    VLOG(1) << str << ": " << supports;
    if (supports) ++count;
  }
  return IntReturn(count);
}

absl::StatusOr<std::vector<std::string>> Day07_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

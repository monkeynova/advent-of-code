#include "advent_of_code/2017/day09/day09.h"

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

int TotalGroupScore(absl::string_view str) {
  int total_score = 0;
  int group_level = 0;
  bool in_garbage = false;
  bool in_garbage_escape = false;

  for (char c : str) {
    VLOG(2) << c << ": group_level" << group_level << ", in_garbage=" << in_garbage
            << ", in_garbage_escape=" << in_garbage_escape;
    if (in_garbage) {
      if (in_garbage_escape) {
        in_garbage_escape = false;
      } else {
        if (c == '>') in_garbage = false;
        if (c == '!') in_garbage_escape = true;
      }
    } else {
      if (c == '{') ++group_level;
      if (c == '}') {
        total_score += group_level;
        --group_level;
      }
      if (c == '<') in_garbage = true;
    }
  }

  return total_score;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day09_2017::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<std::pair<absl::string_view, int>> tests = {
    {"{}", 1},
    {"{{{}}}", 6},
    {"{{}{}}", 5},
    {"{{<a!>},{<a!>},{<a!>},{<ab>}}", 3},
  };
  for (const auto& [str, expect] : tests) {
    int got = TotalGroupScore(str);
    if (got != expect) {
      LOG(ERROR) << "Bad result: " << got << " != " << expect << "; " << str;
    }
  }
  if (input.size() != 1) return Error("Bad input");
  return IntReturn(TotalGroupScore(input[0]));
}

absl::StatusOr<std::vector<std::string>> Day09_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

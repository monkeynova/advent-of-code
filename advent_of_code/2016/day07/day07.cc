#include "advent_of_code/2016/day07/day07.h"

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

bool SupportsTLS(std::string_view str) {
  int level = 0;
  bool match_outside = false;
  bool match_inside = false;
  for (int i = 0; i < str.size(); ++i) {
    if (str[i] == '[') ++level;
    if (str[i] == ']') --level;
    if (i >= 3) {
      if (str[i] == str[i - 3] && str[i - 1] == str[i - 2] &&
          str[i] != str[i - 1]) {
        if (level == 0)
          match_outside = true;
        else
          match_inside = true;
      }
    }
  }
  return match_outside && !match_inside;
}

bool SupportsSSL(std::string_view str) {
  int level = 0;
  absl::flat_hash_set<std::string> outside;
  absl::flat_hash_set<std::string> inside;
  for (int i = 0; i < str.size(); ++i) {
    if (str[i] == '[') ++level;
    if (str[i] == ']') --level;
    if (i >= 2) {
      if (str[i] == str[i - 2] && str[i] != str[i - 1]) {
        if (level == 0) {
          // ABA
          char tmp[] = {str[i], str[i - 1], '\0'};
          outside.insert(std::string(tmp));
        } else {
          // BAB
          char tmp[] = {str[i - 1], str[i], '\0'};
          inside.insert(std::string(tmp));
        }
      }
    }
  }
  VLOG(1) << str << ": " << absl::StrJoin(outside, ",") << "; "
          << absl::StrJoin(inside, ",");
  for (const auto& s : outside) {
    if (inside.contains(s)) return true;
  }
  return false;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_07::Part1(
    absl::Span<std::string_view> input) const {
  int count = 0;
  for (std::string_view str : input) {
    bool supports = SupportsTLS(str);
    VLOG(1) << str << ": " << supports;
    if (supports) ++count;
  }
  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2016_07::Part2(
    absl::Span<std::string_view> input) const {
  int count = 0;
  for (std::string_view str : input) {
    bool supports = SupportsSSL(str);
    VLOG(1) << str << ": " << supports;
    if (supports) ++count;
  }
  return AdventReturn(count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/7,
    []() { return std::unique_ptr<AdventDay>(new Day_2016_07()); });

}  // namespace advent_of_code

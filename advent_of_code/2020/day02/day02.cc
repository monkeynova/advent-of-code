#include "advent_of_code/2020/day02/day02.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_02::Part1(
    absl::Span<std::string_view> input) const {
  static LazyRE2 pattern{"(\\d+)-(\\d+) (.): (.*)"};
  int valid = 0;
  for (std::string_view str : input) {
    int min;
    int max;
    std::string c;
    std::string password;
    if (!RE2::FullMatch(str, *pattern, &min, &max, &c, &password)) {
      return absl::InternalError("");
    }
    int count = 0;
    for (int i = 0; i < password.size(); ++i) {
      if (password[i] == c[0]) ++count;
    }
    if (count >= min && count <= max) {
      ++valid;
    }
  }
  return AdventReturn(valid);
}

absl::StatusOr<std::string> Day_2020_02::Part2(
    absl::Span<std::string_view> input) const {
  static LazyRE2 pattern{"(\\d+)-(\\d+) (.): (.*)"};
  int valid = 0;
  for (std::string_view str : input) {
    int min;
    int max;
    std::string c;
    std::string password;
    if (!RE2::FullMatch(str, *pattern, &min, &max, &c, &password)) {
      return absl::InternalError("");
    }
    if ((password[min - 1] == c[0]) ^ (password[max - 1] == c[0])) {
      ++valid;
    }
  }
  return AdventReturn(valid);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/2,
    []() { return std::unique_ptr<AdventDay>(new Day_2020_02()); });

}  // namespace advent_of_code

#include "advent_of_code/2020/day2/day2.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day2_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  RE2 pattern{"(\\d+)-(\\d+) (.): (.*)"};
  int valid = 0;
  for (absl::string_view str : input) {
    int min;
    int max;
    std::string c;
    std::string password;
    if (!RE2::FullMatch(str, pattern, &min, &max, &c, &password)) {
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
  return std::vector<std::string>{absl::StrCat(valid)};
}

absl::StatusOr<std::vector<std::string>> Day2_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  RE2 pattern{"(\\d+)-(\\d+) (.): (.*)"};
  int valid = 0;
  for (absl::string_view str : input) {
    int min;
    int max;
    std::string c;
    std::string password;
    if (!RE2::FullMatch(str, pattern, &min, &max, &c, &password)) {
      return absl::InternalError("");
    }
    if (password[min - 1] == c[0] ^ password[max - 1] == c[0]) {
      ++valid;
    }
  }
  return std::vector<std::string>{absl::StrCat(valid)};
}

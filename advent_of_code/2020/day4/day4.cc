#include "advent_of_code/2020/day4/day4.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

bool Valid(const absl::flat_hash_map<std::string, std::string>& passport) {
  for(absl::string_view test : {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"}) {
    if (!passport.contains(test)) return false;
  }
  return true;
}

absl::StatusOr<std::vector<std::string>> Day4_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::flat_hash_map<std::string, std::string> passport;
  int valid = 0;
  for (absl::string_view line : input) {
    if (line.empty()) {
      if (Valid(passport)) {
        ++valid;
      }
      passport.clear();
    }
    std::vector<absl::string_view> records = absl::StrSplit(line, " ");
    for (absl::string_view r : records) {
      if (r.empty()) continue;
      std::vector<absl::string_view> kv = absl::StrSplit(r, ":");
      if (kv.size() != 2) return absl::InvalidArgumentError(absl::StrCat("bad k/v: ", r));
      passport[kv[0]] = passport[kv[1]];
    }
  }
      if (Valid(passport)) {
        ++valid;
      }
  return std::vector<std::string>{absl::StrCat(valid)};
}

absl::StatusOr<std::vector<std::string>> Day4_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}

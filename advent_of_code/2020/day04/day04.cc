#include "advent_of_code/2020/day04/day04.h"

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

absl::Status Valid(
    const absl::flat_hash_map<std::string, std::string>& passport) {
  for (absl::string_view test :
       {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"}) {
    if (!passport.contains(test)) {
      return absl::InvalidArgumentError(absl::StrCat("Missing field: ", test));
    }
  }
  return absl::OkStatus();
  ;
}

absl::Status Valid2(absl::flat_hash_map<std::string, std::string> passport) {
  if (absl::Status st = Valid(passport); !st.ok()) return st;
  int year;
  if (!absl::SimpleAtoi(passport["byr"], &year)) {
    return absl::InvalidArgumentError("byr isn't numeric");
  }
  if (year < 1920 || year > 2002) {
    return absl::InvalidArgumentError("byr is out of range");
  }
  if (!absl::SimpleAtoi(passport["iyr"], &year)) {
    return absl::InvalidArgumentError("iyr isn't numeric");
  }
  if (year < 2010 || year > 2020) {
    return absl::InvalidArgumentError("iyr is out of range");
  }
  if (!absl::SimpleAtoi(passport["eyr"], &year)) {
    return absl::InvalidArgumentError("eyr isn't numeric");
  }
  if (year < 2020 || year > 2030) {
    return absl::InvalidArgumentError("eyr is out of range");
  }
  int val;
  if (RE2::FullMatch(passport["hgt"], "(\\d+)cm", &val)) {
    if (val < 150 || val > 193) {
      return absl::InvalidArgumentError("hgt(cm) is out of range");
    }
  } else if (RE2::FullMatch(passport["hgt"], "(\\d+)in", &val)) {
    if (val < 59 || val > 76) {
      return absl::InvalidArgumentError("hgt(in) is out of range");
    }
  } else {
    return absl::InvalidArgumentError("hgt isn't cm or in");
  }
  if (!RE2::FullMatch(passport["hcl"], "#[0-9a-f]{6}")) {
    return absl::InvalidArgumentError("hcl format");
  }
  absl::flat_hash_set<std::string> valid_ecl = {"amb", "blu", "brn", "gry",
                                                "grn", "hzl", "oth"};
  if (!valid_ecl.contains(passport["ecl"])) {
    return absl::InvalidArgumentError("ecl ins't in allowlist");
  }
  if (!RE2::FullMatch(passport["pid"], "\\d{9}")) {
    return absl::InvalidArgumentError("pid isn't numeric");
  }

  return absl::OkStatus();
  ;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_04::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<std::string, std::string> passport;
  int valid = 0;
  for (absl::string_view line : input) {
    if (line.empty()) {
      if (Valid(passport).ok()) {
        ++valid;
      }
      passport.clear();
    }
    for (absl::string_view r : absl::StrSplit(line, " ")) {
      if (r.empty()) continue;
      const auto [it, inserted] = passport.emplace(PairSplit(r, ":"));
      if (!inserted) return Error("Duplicate key: ", r);
    }
  }
  if (Valid(passport).ok()) {
    ++valid;
  }
  return IntReturn(valid);
}

absl::StatusOr<std::string> Day_2020_04::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<std::string, std::string> passport;
  int valid = 0;
  for (absl::string_view line : input) {
    if (line.empty()) {
      if (Valid2(passport).ok()) {
        ++valid;
      }
      passport.clear();
    }
    for (absl::string_view r : absl::StrSplit(line, " ")) {
      if (r.empty()) continue;
      const auto [it, inserted] = passport.emplace(PairSplit(r, ":"));
      if (!inserted) return Error("Duplicate key: ", r);
    }
  }
  if (Valid2(passport).ok()) {
    ++valid;
  }
  return IntReturn(valid);
}

}  // namespace advent_of_code

#include "advent_of_code/2020/day4/day4.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

bool Valid(const absl::flat_hash_map<std::string, std::string>& passport) {
  for (absl::string_view test :
       {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"}) {
    if (!passport.contains(test)) return false;
  }
  return true;
}

std::string Valid2(absl::flat_hash_map<std::string, std::string> passport) {
  for (absl::string_view test :
       {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"}) {
    if (!passport.contains(test)) return "contains";
  }
  int year;
  if (!absl::SimpleAtoi(passport["byr"], &year)) return passport["byr"];
  if (year < 1920 || year > 2002) return "byr";
  if (!absl::SimpleAtoi(passport["iyr"], &year)) return "iyr atoi";
  if (year < 2010 || year > 2020) return "iyr";
  if (!absl::SimpleAtoi(passport["eyr"], &year)) return "eyr atoi";
  if (year < 2020 || year > 2030) return "eyr";
  int val;
  if (RE2::FullMatch(passport["hgt"], "(\\d+)cm", &val)) {
    if (val < 150 || val > 193) return "hgt";
  } else if (RE2::FullMatch(passport["hgt"], "(\\d+)in", &val)) {
    if (val < 59 || val > 76) return "hgt";
  } else {
    return "hgt";
  }
  if (!RE2::FullMatch(passport["hcl"], "#[0-9a-f]{6}")) return "hcl";
  absl::flat_hash_set<std::string> valid_ecl = {"amb", "blu", "brn", "gry",
                                                "grn", "hzl", "oth"};
  if (!valid_ecl.contains(passport["ecl"])) return "ecl";
  if (!RE2::FullMatch(passport["pid"], "\\d{9}")) return "pid";

  return "";
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
      if (kv.size() != 2)
        return absl::InvalidArgumentError(absl::StrCat("bad k/v: ", r));
      passport[kv[0]] = kv[1];
    }
  }
  if (Valid(passport)) {
    ++valid;
  }
  return std::vector<std::string>{absl::StrCat(valid)};
}

absl::StatusOr<std::vector<std::string>> Day4_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::flat_hash_map<std::string, std::string> passport;
  int valid = 0;
  for (absl::string_view line : input) {
    if (line.empty()) {
      std::string str = Valid2(passport);
      LOG(WARNING) << str;
      LOG(WARNING) << "{"
                   << absl::StrJoin(
                          passport, ",",
                          [](std::string* out,
                             const std::pair<std::string, std::string>& kv) {
                            absl::StrAppend(out, kv.first, ": ", kv.second);
                          });
      if (str.empty()) {
        ++valid;
      }
      passport.clear();
    }
    std::vector<absl::string_view> records = absl::StrSplit(line, " ");
    for (absl::string_view r : records) {
      if (r.empty()) continue;
      std::vector<absl::string_view> kv = absl::StrSplit(r, ":");
      if (kv.size() != 2)
        return absl::InvalidArgumentError(absl::StrCat("bad k/v: ", r));
      passport[kv[0]] = kv[1];
    }
  }
  std::string str = Valid2(passport);
  LOG(WARNING) << str;
  LOG(WARNING) << "{"
               << absl::StrJoin(
                      passport, ",",
                      [](std::string* out,
                         const std::pair<std::string, std::string>& kv) {
                        absl::StrAppend(out, kv.first, ": ", kv.second);
                      });
  if (str.empty()) {
    ++valid;
  }
  return std::vector<std::string>{absl::StrCat(valid)};
}

#include "advent_of_code/2020/day1/day1.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

absl::StatusOr<std::vector<std::string>> Day1_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  std::vector<int> list;
  for (absl::string_view str : input) {
    list.push_back(0);
    if (!absl::SimpleAtoi(str, &list.back())) return absl::InvalidArgumentError("bad int");
  }
  int res = -1;
  for (int a : list) {
    for (int b : list) {
      if (a+b == 2020) {
        res = a*b;
      }
    }
  }
  if (res == -1) return absl::InvalidArgumentError("nothing found");
  return std::vector<std::string>{absl::StrCat(res)};
}

absl::StatusOr<std::vector<std::string>> Day1_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  std::vector<int> list;
  for (absl::string_view str : input) {
    list.push_back(0);
    if (!absl::SimpleAtoi(str, &list.back())) return absl::InvalidArgumentError("bad int");
  }
  int res = -1;
  for (int a : list) {
    for (int b : list) {
      for (int c : list) {
      if (a+b+c == 2020) {
        res = a*b*c;
      }
      }
    }
  }
  if (res == -1) return absl::InvalidArgumentError("nothing found");
  return std::vector<std::string>{absl::StrCat(res)};
}

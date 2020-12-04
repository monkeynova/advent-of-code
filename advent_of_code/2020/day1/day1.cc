#include "advent_of_code/2020/day1/day1.h"

#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

absl::StatusOr<std::vector<std::string>> Day1_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  std::vector<int> list;
  for (absl::string_view str : input) {
    list.push_back(0);
    if (!absl::SimpleAtoi(str, &list.back()))
      return absl::InvalidArgumentError("bad int");
  }
  int res = -1;
  absl::flat_hash_set<int> hist;
  for (int a : list) {
    if (hist.contains(2020 - a)) {
      res = a * (2020 - a);
      break;
    }
    hist.insert(a);
  }
  if (res == -1) return absl::InvalidArgumentError("nothing found");
  return std::vector<std::string>{absl::StrCat(res)};
}

absl::StatusOr<std::vector<std::string>> Day1_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  std::vector<int> list;
  for (absl::string_view str : input) {
    list.push_back(0);
    if (!absl::SimpleAtoi(str, &list.back())) {
      return absl::InvalidArgumentError("bad int");
    }
  }
  int res = -1;
  absl::flat_hash_set<int> hist;
  for (int i = 0; i < list.size(); ++i) {
    int a = list[i];
    for (int j = 0; j < list.size(); ++j) {
      int b = list[j];
      if (hist.contains(2020 - a - b)) {
        res = a * b * (2020 - a - b);
        break;
      }
    }
    hist.insert(a);
  }
  if (res == -1) return absl::InvalidArgumentError("nothing found");
  return std::vector<std::string>{absl::StrCat(res)};
}

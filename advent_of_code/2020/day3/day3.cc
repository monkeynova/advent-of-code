#include "advent_of_code/2020/day3/day3.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day3_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  int product = 1;
  for (Point dir : {Point{3,1}}) {
    int trees = 0;
    for (Point pos = {0, 0}; pos.y < input.size(); pos += dir) {
      pos.x = pos.x % input[pos.y].size();
       if (input[pos.y][pos.x] == '#') ++trees;
    }
    product *= trees;
  }
  return std::vector<std::string>{absl::StrCat(product)};
}

absl::StatusOr<std::vector<std::string>> Day3_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  int64_t product = 1;
  for (Point dir : {Point{1, 1}, Point{3,1}, Point{5,1}, Point{7,1}, Point{1, 2}}) {
    int64_t trees = 0;
    for (Point pos = {0, 0}; pos.y < input.size(); pos += dir) {
      pos.x = pos.x % input[pos.y].size();
       if (input[pos.y][pos.x] == '#') ++trees;
    }
    product *= trees;
  }
  return std::vector<std::string>{absl::StrCat(product)};
}

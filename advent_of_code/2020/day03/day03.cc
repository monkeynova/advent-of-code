#include "advent_of_code/2020/day03/day03.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {}  // namespace

absl::StatusOr<std::vector<std::string>> Day03_2020::Part1(
    absl::Span<absl::string_view> input) const {
  int product = 1;
  for (Point dir : {Point{3, 1}}) {
    int trees = 0;
    for (Point pos = Cardinal::kOrigin; pos.y < input.size(); pos += dir) {
      pos.x = pos.x % input[pos.y].size();
      if (input[pos.y][pos.x] == '#') ++trees;
    }
    product *= trees;
  }
  return IntReturn(product);
}

absl::StatusOr<std::vector<std::string>> Day03_2020::Part2(
    absl::Span<absl::string_view> input) const {
  int64_t product = 1;
  for (Point dir :
       {Point{1, 1}, Point{3, 1}, Point{5, 1}, Point{7, 1}, Point{1, 2}}) {
    int64_t trees = 0;
    for (Point pos = Cardinal::kOrigin; pos.y < input.size(); pos += dir) {
      pos.x = pos.x % input[pos.y].size();
      if (input[pos.y][pos.x] == '#') ++trees;
    }
    product *= trees;
  }
  return IntReturn(product);
}

}  // namespace advent_of_code

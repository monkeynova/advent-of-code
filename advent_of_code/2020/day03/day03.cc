#include "advent_of_code/2020/day03/day03.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_03::Part1(
    absl::Span<std::string_view> input) const {
  int product = 1;
  for (Point dir : {Point{3, 1}}) {
    int trees = 0;
    for (Point pos = Cardinal::kOrigin; pos.y < input.size(); pos += dir) {
      pos.x = pos.x % input[pos.y].size();
      if (input[pos.y][pos.x] == '#') ++trees;
    }
    product *= trees;
  }
  return AdventReturn(product);
}

absl::StatusOr<std::string> Day_2020_03::Part2(
    absl::Span<std::string_view> input) const {
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
  return AdventReturn(product);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/3, []() {
  return std::unique_ptr<AdventDay>(new Day_2020_03());
});

}  // namespace advent_of_code

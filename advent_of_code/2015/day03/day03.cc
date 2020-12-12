#include "advent_of_code/2015/day03/day03.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day03_2015::Part1(
    const std::vector<absl::string_view>& input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  absl::flat_hash_map<Point, int> visit;
  Point p{0, 0};
  for (char c : input[0]) {
    ++visit[p];
    switch(c) {
      case '^': p += Cardinal::kNorth; break;
      case 'v': p += Cardinal::kSouth; break;
      case '<': p += Cardinal::kWest; break;
      case '>': p += Cardinal::kEast; break;
      default: return absl::InvalidArgumentError("Bad direction");
    }
  }
  return IntReturn(visit.size());
}

absl::StatusOr<std::vector<std::string>> Day03_2015::Part2(
    const std::vector<absl::string_view>& input) const {
  return IntReturn(-1);
}

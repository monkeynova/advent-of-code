#include "advent_of_code/2020/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day24_2020::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<Point, bool> grid;
  for (absl::string_view str : input) {
    Point p = {0, 0};
    for (int i = 0; i < str.size(); ++i) {
      if (str[i] == 'w') {
        p += 2 * Cardinal::kWest;
      } else if (str[i] == 'e') {
        p += 2 * Cardinal::kEast;
      } else if (str[i] == 'n') {
        if (i + 1 == str.size()) return Error("Bad sequence (@", i, "): ", str);
        if (str[i+1] == 'w') {
          p += Cardinal::kNorthWest;
        } else if (str[i+1] == 'e') {
          p += Cardinal::kNorthEast;
        } else {
          return Error("Bad sequence (@", i, "): ", str);
        }
        ++i;
      } else if (str[i] == 's') {
        if (i + 1 == str.size()) return Error("Bad sequence (@", i, "): ", str);
        if (str[i+1] == 'w') {
          p += Cardinal::kSouthWest;
        } else if (str[i+1] == 'e') {
          p += Cardinal::kSouthEast;
        } else {
          return Error("Bad sequence (@", i, "): ", str);
        }
        ++i;
      } else {
        return Error("Bad sequence (@", i, "): ", str);
      }
    }
    grid[p] = !grid[p];
  }
  int black_count = 0;
  for (const auto& [_, is_black] : grid) {
    if (is_black) ++black_count;
  }
  return IntReturn(black_count);
}

absl::StatusOr<std::vector<std::string>> Day24_2020::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

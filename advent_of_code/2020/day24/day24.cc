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

absl::StatusOr<absl::flat_hash_map<Point, bool>> ParseGrid(
    absl::Span<absl::string_view> input) {
  absl::flat_hash_map<Point, bool> grid;
  for (absl::string_view str : input) {
    Point p = {0, 0};
    for (int i = 0; i < str.size(); ++i) {
      if (str[i] == 'w') {
        p += 2 * Cardinal::kWest;
      } else if (str[i] == 'e') {
        p += 2 * Cardinal::kEast;
      } else if (str[i] == 'n') {
        if (i + 1 == str.size())
          return AdventDay::Error("Bad sequence (@", i, "): ", str);
        if (str[i + 1] == 'w') {
          p += Cardinal::kNorthWest;
        } else if (str[i + 1] == 'e') {
          p += Cardinal::kNorthEast;
        } else {
          return AdventDay::Error("Bad sequence (@", i, "): ", str);
        }
        ++i;
      } else if (str[i] == 's') {
        if (i + 1 == str.size())
          return AdventDay::Error("Bad sequence (@", i, "): ", str);
        if (str[i + 1] == 'w') {
          p += Cardinal::kSouthWest;
        } else if (str[i + 1] == 'e') {
          p += Cardinal::kSouthEast;
        } else {
          return AdventDay::Error("Bad sequence (@", i, "): ", str);
        }
        ++i;
      } else {
        return AdventDay::Error("Bad sequence (@", i, "): ", str);
      }
    }
    grid[p] = !grid[p];
  }
  return grid;
}

absl::StatusOr<absl::flat_hash_map<Point, bool>> RunStep(
    const absl::flat_hash_map<Point, bool>& grid) {
  const std::vector<Point> kDirs = {2 * Cardinal::kWest,  2 * Cardinal::kEast,
                                    Cardinal::kSouthWest, Cardinal::kSouthEast,
                                    Cardinal::kNorthWest, Cardinal::kNorthEast};

  absl::flat_hash_map<Point, int> neighbors;
  for (const auto& [p, is_black] : grid) {
    if (!is_black) continue;
    for (Point d : kDirs) {
      ++neighbors[p + d];
    }
  }

  absl::flat_hash_map<Point, bool> ret;
  for (const auto& [p, count] : neighbors) {
    bool is_black = false;
    if (auto it = grid.find(p); it != grid.end()) is_black = it->second;
    if (is_black) {
      if (count == 1 || count == 2) {
        ret[p] = true;
      }
    } else if (count == 2) {
      ret[p] = true;
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<Point, bool>> grid = ParseGrid(input);
  if (!grid.ok()) return grid.status();
  int black_count = 0;
  for (const auto& [_, is_black] : *grid) {
    if (is_black) ++black_count;
  }
  return IntReturn(black_count);
}

absl::StatusOr<std::string> Day_2020_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<Point, bool>> grid = ParseGrid(input);
  if (!grid.ok()) return grid.status();

  for (int i = 0; i < 100; ++i) {
    grid = RunStep(*grid);
    if (!grid.ok()) return grid.status();
  }

  int black_count = 0;
  for (const auto& [_, is_black] : *grid) {
    if (is_black) ++black_count;
  }
  return IntReturn(black_count);
}

}  // namespace advent_of_code

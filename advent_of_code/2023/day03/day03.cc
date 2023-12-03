// http://adventofcode.com/2023/day/3

#include "advent_of_code/2023/day03/day03.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

bool IsDigit(char c) {
  return c >= '0' && c <= '9';
}

// Returns a pair of points, {min, max}, with the same y value as `start` and
// such that all points with x values in [min.x, max.x) are on `b` and have a
// digit stored in the correspondng board location.
// TODO(@monkeynova): whole_year<1sec challenge: Consider a std::string_view
// model directly off of b.stride().
std::pair<Point, Point> DigitRange(const CharBoard& b, Point start) {
  Point min = start;
  while (b.OnBoard(min) && IsDigit(b[min])) {
    min += Cardinal::kWest;
  }
  min += Cardinal::kEast;
  Point max = start + Cardinal::kEast;
  while (b.OnBoard(max) && IsDigit(b[max])) {
    max += Cardinal::kEast;
  }
  return {min, max};
}

}  // namespace

absl::StatusOr<std::string> Day_2023_03::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  int sum = 0;
  std::vector<bool> used(b.range().Area(), false);
  auto point_idx = [&](Point p) {
    return p.y * b.width() + p.x;
  };
  for (auto [p, c] : b) {
    if (c == '.' || IsDigit(c)) continue;
    for (Point dir : Cardinal::kEightDirs) {
      Point t = p + dir;
      if (!b.OnBoard(t)) continue;
      if (!IsDigit(b[t])) continue;
      if (used[point_idx(t)]) continue;

      auto [min, max] = DigitRange(b, t);
      int num = 0;
      for (Point d = min; d != max; d += Cardinal::kEast) {
        used[point_idx(d)] = true;
        num = num * 10 + b[d] - '0';
      }
      sum += num;
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2023_03::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  int64_t sum = 0;
  std::vector<bool> used(b.range().Area(), false);
  auto point_idx = [&](Point p) {
    return p.y * b.width() + p.x;
  };
  for (auto [p, c] : b) {
    if (c != '*') continue;
    std::vector<int64_t> adjacent;
    for (Point dir : Cardinal::kEightDirs) {
      Point t = p + dir;
      if (!b.OnBoard(t)) continue;
      if (!IsDigit(b[t])) continue;
      if (used[point_idx(t)]) continue;

      auto [min, max] = DigitRange(b, t);
      int num = 0;
      for (Point d = min; d != max; d += Cardinal::kEast) {
        used[point_idx(d)] = true;
        num = num * 10 + b[d] - '0';
      }
      adjacent.push_back(num);
    }
    if (adjacent.size() == 2) {
      sum += adjacent[0] * adjacent[1];
    }
  }
  return AdventReturn(sum);
}

}  // namespace advent_of_code

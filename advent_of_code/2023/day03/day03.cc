// http://adventofcode.com/2023/day/3

#include "advent_of_code/2023/day03/day03.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

// Returns a tuple, {min_x, max_x, num}, such that all points with y values
// equal to start.y and x values in [min_x, max_x) are on `b` and have a
// digit stored in the correspondng board location. num is the value of the
// integer contained within that range of digits.
std::tuple<int, int, int> DigitRange(const ImmutableCharBoard& b, Point start) {
  std::string_view row = b.row(start.y);
  int num = 0;
  int min = start.x;
  int pow10 = 1;
  while (min >= 0 && isdigit(row[min])) {
    num += (row[min] - '0') * pow10;
    pow10 *= 10;
    --min;
  }
  ++min;
  int max = start.x + 1;
  while (max < row.size() && isdigit(row[max])) {
    num = num * 10 + (row[max] - '0');
    ++max;
  }
  return {min, max, num};
}

}  // namespace

absl::StatusOr<std::string> Day_2023_03::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  int sum = 0;
  std::vector<bool> used(b.range().Area(), false);
  auto point_idx = [&](Point p) { return p.y * b.width() + p.x; };
  std::bitset<256> find = ~std::bitset<256>();
  find['.'] = false;
  find['\n'] = false;
  for (char c = '0'; c <= '9'; ++c) find[c] = false;
  for (Point p : b.Find(find)) {
    for (Point dir : Cardinal::kEightDirs) {
      Point t = p + dir;
      if (!b.OnBoard(t)) continue;
      if (!isdigit(b[t])) continue;
      if (used[point_idx(t)]) continue;

      auto [min, max, num] = DigitRange(b, t);
      int used_idx = point_idx({min, t.y});
      for (int x = min; x != max; ++x, ++used_idx) {
        used[used_idx] = true;
      }
      sum += num;
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2023_03::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  int64_t sum = 0;
  std::vector<bool> used(b.range().Area(), false);
  auto point_idx = [&](Point p) { return p.y * b.width() + p.x; };
  for (Point p : b.Find('*')) {
    std::array<int64_t, 3> adjacent;
    int adj_idx = 0;
    for (Point dir : Cardinal::kEightDirs) {
      Point t = p + dir;
      if (!b.OnBoard(t)) continue;
      if (!isdigit(b[t])) continue;
      if (used[point_idx(t)]) continue;

      auto [min, max, num] = DigitRange(b, t);
      int used_idx = point_idx({min, t.y});
      for (int x = min; x != max; ++x, ++used_idx) {
        used[used_idx] = true;
      }
      adjacent[adj_idx] = num;
      if (++adj_idx == 3) break;
    }
    if (adj_idx == 2) {
      sum += adjacent[0] * adjacent[1];
    }
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/3,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_03()); });

}  // namespace advent_of_code

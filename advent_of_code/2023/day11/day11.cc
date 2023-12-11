// http://adventofcode.com/2023/day/11

#include "advent_of_code/2023/day11/day11.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

void Expand(std::vector<Point>& galaxies, int width, int height, int size) {
  std::vector<int> x_expand(width, false);
  std::vector<int> y_expand(height, false);
  for (Point p : galaxies) {
    x_expand[p.x] = 1;
    y_expand[p.y] = 1;
  }
  {
    int total = 0;
    for (int& i : x_expand) {
      if (i == 0) total += size - 1;
      i = total;
    }
  }
  {
    int total = 0;
    for (int& i : y_expand) {
      if (i == 0) total += size - 1;
      i = total;
    }
  }
  for (Point& p : galaxies) {
    p.x += x_expand[p.x];
    p.y += y_expand[p.y];
  }
}

int64_t TotalPairDist(const std::vector<Point>& galaxies) {
  int64_t total_dist = 0;
  for (int i = 0; i < galaxies.size(); ++i) {
    Point p1 = galaxies[i];
    for (int j = i + 1; j < galaxies.size(); ++j) {
      Point p2 = galaxies[j];
      VLOG(2) << p1 << "-" << p2 << " = " << (p1 - p2).dist();
      total_dist += (p1 - p2).dist();
    }
  }
  return total_dist;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_11::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  std::vector<Point> galaxies = b.FindVec('#');
  Expand(galaxies, b.width(), b.height(), 2);
  return AdventReturn(TotalPairDist(galaxies));
}

absl::StatusOr<std::string> Day_2023_11::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(int64_t expand, IntParam());
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  std::vector<Point> galaxies = b.FindVec('#');
  Expand(galaxies, b.width(), b.height(), expand);
  return AdventReturn(TotalPairDist(galaxies));
}

}  // namespace advent_of_code

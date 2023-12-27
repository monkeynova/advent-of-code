// http://adventofcode.com/2022/day/14

#include "advent_of_code/2022/day14/day14.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

constexpr Point kAddAt = {500, 0};

absl::StatusOr<absl::flat_hash_set<Point>> AllPoints(
    absl::Span<std::string_view> input) {
  absl::flat_hash_set<Point> ret;
  for (std::string_view line : input) {
    std::optional<Point> last;
    for (std::string_view p_str : absl::StrSplit(line, " -> ")) {
      Point p;
      if (!Point::RE2Parse(p_str.data(), p_str.size(), &p)) {
        return Error("Bad point");
      }
      if (last) {
        Point d = (p - *last).min_step();
        if (d.dist() != 1) return Error("Bad d");
        for (Point t = *last; t != p; t += d) {
          ret.insert(t);
        }
      }
      ret.insert(p);
      last = p;
    }
  }
  return ret;
}

CharBoard DrawGrid(PointRectangle grid,
                   const absl::flat_hash_set<Point> points) {
  CharBoard b(grid.max.x - grid.min.x + 1, grid.max.y - grid.min.y + 1);
  for (Point p : points) {
    b[p - grid.min] = '#';
  }
  return b;
}

absl::StatusOr<bool> AddSand(CharBoard& b, Point at) {
  if (!b.OnBoard(at)) return Error("Not on board: ", at);
  if (b[at] != '.') return false;
  while (b.OnBoard(at + Cardinal::kSouth)) {
    bool fell = false;
    for (Point d :
         {Cardinal::kSouth, Cardinal::kSouthWest, Cardinal::kSouthEast}) {
      Point t = at + d;
      if (!b.OnBoard(at)) return Error("Not on board: ", t);
      if (b[t] == '.') {
        at = t;
        fell = true;
        break;
      }
    }
    if (!fell) {
      b[at] = 'o';
      return true;
    }
  }
  return false;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_14::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(absl::flat_hash_set<Point> points, AllPoints(input));

  PointRectangle grid{kAddAt, kAddAt};
  for (Point p : points) grid.ExpandInclude(p);

  // Add space to fall off the end.
  --grid.min.x;
  ++grid.max.x;
  VLOG(1) << "Grid: " << grid;

  CharBoard b = DrawGrid(grid, points);
  VLOG(2) << "Board:\n" << b;

  Point add = kAddAt - grid.min;
  int add_count = 0;
  while (b[add] == '.') {
    ASSIGN_OR_RETURN(bool added, AddSand(b, add));
    if (!added) break;
    VLOG(2) << "Board:\n" << b;
    ++add_count;
  }
  return AdventReturn(add_count);
}

absl::StatusOr<std::string> Day_2022_14::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(absl::flat_hash_set<Point> points, AllPoints(input));

  PointRectangle grid{kAddAt, kAddAt};
  for (Point p : points) grid.ExpandInclude(p);

  // Expand to include the bottom line.
  grid.ExpandInclude(Point{kAddAt.x, grid.max.y + 2});
  // Extend the bottom line until we have space for a full pyramid.
  grid.ExpandInclude(Point{kAddAt.x - grid.max.y, grid.max.y});
  grid.ExpandInclude(Point{kAddAt.x + grid.max.y, grid.max.y});
  VLOG(1) << "Grid: " << grid;

  CharBoard b = DrawGrid(grid, points);
  // Draw the bottom line.
  for (int x = 0; x < b.width(); ++x) {
    b[Point{x, b.height() - 1}] = '#';
  }

  VLOG(2) << "Board:\n" << b;
  Point add = kAddAt - grid.min;
  int add_count = 0;
  while (b[add] == '.') {
    ASSIGN_OR_RETURN(bool added, AddSand(b, add));
    if (!added) break;
    VLOG(2) << "Board:\n" << b;
    ++add_count;
  }
  return AdventReturn(add_count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/14, []() {
  return std::unique_ptr<AdventDay>(new Day_2022_14());
});

}  // namespace advent_of_code

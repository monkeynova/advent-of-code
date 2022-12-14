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
#include "re2/re2.h"

namespace advent_of_code {

namespace {

constexpr Point kAddAt = {500, 0};

absl::StatusOr<PointRectangle> FindGrid(absl::Span<absl::string_view> input) {
  PointRectangle grid{kAddAt, kAddAt};
  for (absl::string_view line : input) {
    for (absl::string_view p_str : absl::StrSplit(line, " -> ")) {
      Point p;
      if (!Point::RE2Parse(p_str.data(), p_str.size(), &p)) {
        return Error("Bad point");
      } 
      grid.ExpandInclude(p);
    }
  }
  return grid;
}

absl::StatusOr<CharBoard> DrawGrid(PointRectangle grid,
                                   absl::Span<absl::string_view> input) {
  CharBoard b(grid.max.x - grid.min.x + 1, grid.max.y - grid.min.y + 1);
  for (absl::string_view line : input) {
    std::optional<Point> last;
    for (absl::string_view p_str : absl::StrSplit(line, " -> ")) {
      Point p;
      if (!Point::RE2Parse(p_str.data(), p_str.size(), &p)) {
        return Error("Bad point");
      } 
      if (last) {
        Point d = (p - *last).min_step();
        if (d.dist() != 1) return Error("Bad d");
        for (Point t = *last; t != p; t += d) {
          b[t - grid.min] = '#';
        }
      }
      b[p - grid.min] = '#';
      last = p;
    }
  }
  return b;
}

bool AddSand(CharBoard& b, Point at) {
  CHECK(b.OnBoard(at));
  if (b[at] != '.') return false;
  bool fell = true;
  while (fell) {
    if (!b.OnBoard(at + Cardinal::kSouth)) return false;

    fell = false;
    for (Point d : {Cardinal::kSouth, Cardinal::kSouthWest, Cardinal::kSouthEast}) {
      Point t = at + d;
      CHECK(b.OnBoard(t));
      if (b[t] == '.') {
        at = t;
        fell = true;
        break;
      }
    }
  }
  b[at] = 'o';
  return true;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_14::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<PointRectangle> grid = FindGrid(input);
  if (!grid.ok()) return grid.status();

  // Add space to fall off the end.
  --grid->min.x;
  ++grid->max.x;
  VLOG(1) << "Grid: " << *grid;

  absl::StatusOr<CharBoard> b = DrawGrid(*grid, input);
  if (!b.ok()) return b.status();
  VLOG(2) << "Board:\n" << *b;

  Point add = kAddAt - grid->min;
  int added = 0;
  while ((*b)[add] == '.') {
    if (!AddSand(*b, add)) break;
    VLOG(2) << "Board:\n" << *b;
    ++added;
  }
  return IntReturn(added);
}

absl::StatusOr<std::string> Day_2022_14::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<PointRectangle> grid = FindGrid(input);
  if (!grid.ok()) return grid.status();

  // Expand to include the bottom line.
  grid->ExpandInclude(Point{kAddAt.x, grid->max.y + 2});
  // Extend the bottom line until we have space for a full pyramid.
  grid->ExpandInclude(Point{kAddAt.x - grid->max.y, grid->max.y});
  grid->ExpandInclude(Point{kAddAt.x + grid->max.y, grid->max.y});
  VLOG(1) << "Grid: " << *grid;

  absl::StatusOr<CharBoard> b = DrawGrid(*grid, input);
  if (!b.ok()) return b.status();
  // Draw the bottom line.
  for (int x = 0; x < b->width(); ++x) {
    (*b)[Point{x, b->height() - 1}] = '#';
  }

  VLOG(2) << "Board:\n" << *b;
  Point add = kAddAt - grid->min;
  int added = 0;
  while ((*b)[add] == '.') {
    if (!AddSand(*b, add)) break;
    VLOG(2) << "Board:\n" << *b;
    ++added;
  }
  return IntReturn(added);
}

}  // namespace advent_of_code

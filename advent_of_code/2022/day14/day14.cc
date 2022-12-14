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

bool AddSand(CharBoard& b, Point at) {
  if (b[at] != '.') return false;
  while (true) {
    CHECK(b.OnBoard(at));
    if (!b.OnBoard(at + Cardinal::kSouth)) {
      return false;
    } if (b[at + Cardinal::kSouth] == '.') {
      at = at + Cardinal::kSouth;
    } else if (b[at + Cardinal::kSouthWest] == '.') {
      at = at + Cardinal::kSouthWest;
    } else if (b[at + Cardinal::kSouthEast] == '.') {
      at = at + Cardinal::kSouthEast;
    } else {
      break;
    }
  }
  b[at] = 'o';
  return true;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_14::Part1(
    absl::Span<absl::string_view> input) const {
  std::optional<PointRectangle> grid;
  for (absl::string_view line : input) {
    for (absl::string_view p_str : absl::StrSplit(line, " -> ")) {
      Point p;
      if (!RE2::FullMatch(p_str, "(.*)", p.Capture())) {
        return Error("Bad point");
      } 
      if (grid) {
        grid->ExpandInclude(p);
        grid->ExpandInclude(Point{p.x, 0});
      }
      else grid = PointRectangle{p, p};
    }
  }
  --grid->min.x;
  ++grid->max.x;
  VLOG(1) << "Grid: " << *grid;
  CharBoard b(grid->max.x - grid->min.x + 1, grid->max.y - grid->min.y + 1);
  for (absl::string_view line : input) {
    std::optional<Point> last;
    for (absl::string_view p_str : absl::StrSplit(line, " -> ")) {
      Point p;
      if (!RE2::FullMatch(p_str, "(.*)", p.Capture())) {
        return Error("Bad point");
      } 
      if (last) {
        Point d = (p - *last).min_step();
        if (d.dist() != 1) return Error("Bad d");
        for (Point t = *last; t != p; t += d) {
          b[t - grid->min] = '#';
        }
      }
      b[p - grid->min] = '#';
      last = p;
    }
  }
  VLOG(2) << "Board:\n" << b;
  Point add = Point{500, 0} - grid->min;
  int added = 0;
  while (b[add] == '.') {
    if (!AddSand(b, add)) break;
    VLOG(2) << "Board:\n" << b;
    ++added;
  }
  return IntReturn(added);
}

absl::StatusOr<std::string> Day_2022_14::Part2(
    absl::Span<absl::string_view> input) const {
  std::optional<PointRectangle> grid;
  for (absl::string_view line : input) {
    for (absl::string_view p_str : absl::StrSplit(line, " -> ")) {
      Point p;
      if (!RE2::FullMatch(p_str, "(.*)", p.Capture())) {
        return Error("Bad point");
      } 
      if (grid) {
        grid->ExpandInclude(p);
        grid->ExpandInclude(Point{p.x, 0});
      }
      else grid = PointRectangle{p, p};
    }
  }
  grid->max.y += 2;
  grid->min.x = std::min(grid->min.x, 498 - grid->max.y);
  grid->max.x = std::max(grid->max.x, 502 + grid->max.y);
  VLOG(1) << "Grid: " << *grid;
  CharBoard b(grid->max.x - grid->min.x + 1, grid->max.y - grid->min.y + 1);
  for (absl::string_view line : input) {
    std::optional<Point> last;
    for (absl::string_view p_str : absl::StrSplit(line, " -> ")) {
      Point p;
      if (!RE2::FullMatch(p_str, "(.*)", p.Capture())) {
        return Error("Bad point");
      } 
      if (last) {
        Point d = (p - *last).min_step();
        if (d.dist() != 1) return Error("Bad d");
        for (Point t = *last; t != p; t += d) {
          b[t - grid->min] = '#';
        }
      }
      b[p - grid->min] = '#';
      last = p;
    }
  }
  for (int x = 0; x < b.width(); ++x) {
    b[Point{x, b.height() - 1}] = '#';
  }
  VLOG(2) << "Board:\n" << b;
  Point add = Point{500, 0} - grid->min;
  int added = 0;
  while (b[add] == '.') {
    if (!AddSand(b, add)) break;
    VLOG(2) << "Board:\n" << b;
    ++added;
  }
  return IntReturn(added);
}

}  // namespace advent_of_code

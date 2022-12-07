// https://adventofcode.com/2018/day/17
//
// --- Day 17: Reservoir Research ---
// ----------------------------------
//
// You arrive in the year 18. If it weren't for the coat you got in 1018,
// you would be very cold: the North Pole base hasn't even been
// constructed.
//
// Rather, it hasn't been constructed yet. The Elves are making a little
// progress, but there's not a lot of liquid water in this climate, so
// they're getting very dehydrated. Maybe there's more underground?
//
// You scan a two-dimensional vertical slice of the ground nearby and
// discover that it is mostly sand with veins of clay. The scan only
// provides data with a granularity of square meters, but it should be
// good enough to determine how much water is trapped there. In the scan,
// x represents the distance to the right, and y represents the distance
// down. There is also a spring of water near the surface at x=500, y=0.
// The scan identifies which square meters are clay (your puzzle input).
//
// For example, suppose your scan shows the following veins of clay:
//
// x=495, y=2..7
// y=7, x=495..501
// x=501, y=3..7
// x=498, y=2..4
// x=506, y=1..2
// x=498, y=10..13
// x=504, y=10..13
// y=13, x=498..504
//
// Rendering clay as #, sand as ., and the water spring as +, and with x
// increasing to the right and y increasing downward, this becomes:
//
// 44444455555555
// 99999900000000
// 45678901234567
// 0 ......+.......
// 1 ............#.
// 2 .#..#.......#.
// 3 .#..#..#......
// 4 .#..#..#......
// 5 .#.....#......
// 6 .#.....#......
// 7 .#######......
// 8 ..............
// 9 ..............
// 10 ....#.....#...
// 11 ....#.....#...
// 12 ....#.....#...
// 13 ....#######...
//
// The spring of water will produce water forever. Water can move through
// sand, but is blocked by clay. Water always moves down when possible,
// and spreads to the left and right otherwise, filling space that has
// clay on both sides and falling out otherwise.
//
// For example, if five squares of water are created, they will flow
// downward until they reach the clay and settle there. Water that has
// come to rest is shown here as ~, while sand through which water has
// passed (but which is now dry again) is shown as |:
//
// ......+.......
// ......|.....#.
// .#..#.|.....#.
// .#..#.|#......
// .#..#.|#......
// .#....|#......
// .#~~~~~#......
// .#######......
// ..............
// ..............
// ....#.....#...
// ....#.....#...
// ....#.....#...
// ....#######...
//
// Two squares of water can't occupy the same location. If another five
// squares of water are created, they will settle on the first five,
// filling the clay reservoir a little more:
//
// ......+.......
// ......|.....#.
// .#..#.|.....#.
// .#..#.|#......
// .#..#.|#......
// .#~~~~~#......
// .#~~~~~#......
// .#######......
// ..............
// ..............
// ....#.....#...
// ....#.....#...
// ....#.....#...
// ....#######...
//
// Water pressure does not apply in this scenario. If another four
// squares of water are created, they will stay on the right side of the
// barrier, and no water will reach the left side:
//
// ......+.......
// ......|.....#.
// .#..#.|.....#.
// .#..#~~#......
// .#..#~~#......
// .#~~~~~#......
// .#~~~~~#......
// .#######......
// ..............
// ..............
// ....#.....#...
// ....#.....#...
// ....#.....#...
// ....#######...
//
// At this point, the top reservoir overflows. While water can reach the
// tiles above the surface of the water, it cannot settle there, and so
// the next five squares of water settle like this:
//
// ......+.......
// ......|.....#.
// .#..#   |   |||...#.
// .#..#~~#|.....
// .#..#~~#|.....
// .#~~~~~#|.....
// .#~~~~~#|.....
// .#######|.....
// ........|.....
// ........|.....
// ....#...|.#...
// ....#...|.#...
// ....#~~~~~#...
// ....#######...
//
// Note especially the leftmost |: the new squares of water can reach
// this tile, but cannot stop there. Instead, eventually, they all fall
// to the right and settle in the reservoir below.
//
// After 10 more squares of water, the bottom reservoir is also full:
//
// ......+.......
// ......|.....#.
// .#..#||||...#.
// .#..#~~#|.....
// .#..#~~#|.....
// .#~~~~~#|.....
// .#~~~~~#|.....
// .#######|.....
// ........|.....
// ........|.....
// ....#~~~~~#...
// ....#~~~~~#...
// ....#~~~~~#...
// ....#######...
//
// Finally, while there is nowhere left for the water to settle, it can
// reach a few more tiles before overflowing beyond the bottom of the
// scanned data:
//
// ......+.......    (line not counted: above minimum y value)
// ......|.....#.
// .#..#||||...#.
// .#..#~~#|.....
// .#..#~~#|.....
// .#~~~~~#|.....
// .#~~~~~#|.....
// .#######|.....
// ........|.....
// ...|||||||||..
// ...|#~~~~~#|..
// ...|#~~~~~#|..
// ...|#~~~~~#|..
// ...|#######|..
// ...|.......|..    (line not counted: below maximum y value)
// ...|.......|..    (line not counted: below maximum y value)
// ...|.......|..    (line not counted: below maximum y value)
//
// How many tiles can be reached by the water? To prevent counting
// forever, ignore tiles with a y coordinate smaller than the smallest y
// coordinate in your scan data or larger than the largest one. Any x
// coordinate is valid. In this example, the lowest y coordinate given is
// 1, and the highest is 13, causing the water spring (in row 0) and the
// water falling off the bottom of the render (in rows 14 through
// infinity) to be ignored.
//
// So, in the example above, counting both water at rest (~) and other
// sand tiles the water can hypothetically reach (|), the total number of
// tiles the water can reach is 57.
//
// How many tiles can the water reach within the range of y values in
// your scan?
//
// --- Part Two ---
// ----------------
//
// After a very long time, the water spring will run dry. How much water
// will be retained?
//
// In the example above, water that won't eventually drain out is shown
// as ~, a total of 29 tiles.
//
// How many water tiles are left after the water spring stops producing
// water and all remaining water not at rest has drained?

#include "advent_of_code/2018/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::Status DropFrom(CharBoard& b, Point start) {
  VLOG(1) << "Drop From: " << start;
  Point cur = start;
  if (b[cur] != '.') return Error("Drop from non-soil");
  while (b[cur] == '.') {
    b[cur] = '|';
    cur += Point{0, 1};
    if (!b.OnBoard(cur)) {
      // Ran off the board. Done.
      return absl::OkStatus();
    }
  }
  switch (b[cur]) {
    case '|': {
      // Hit previous drop fill.
      return absl::OkStatus();
    }
    case '~':
    case '#': {
      // Need to fill, fall through to work.
      break;
    }
    default:
      return Error("Drop to unhandled");
  }
  cur -= Point{0, 1};
  VLOG(1) << "Drop to: " << cur;
  bool filling = true;
  while (filling) {
    VLOG(3) << b;
    PointRectangle fill = {cur, cur};
    while (b[fill.min - Point{1, 0}] != '#') {
      if (b[fill.min + Point{0, 1}] == '.') {
        if (absl::Status st = DropFrom(b, fill.min + Point{0, 1}); !st.ok()) {
          return st;
        }
        if (b[fill.min + Point{0, 1}] != '~' &&
            b[fill.min + Point{0, 1}] != '#') {
          filling = false;
          break;
        }
      }
      if (b[fill.min - Point{1, 0}] == '|') {
        // Hit an existing drop.
        filling = false;
        break;
      }
      fill.min -= Point{1, 0};
      if (!b.OnBoard(fill.min)) return Error("Off edge");
    }
    while (b[fill.max + Point{1, 0}] != '#') {
      if (b[fill.max + Point{0, 1}] == '.') {
        if (absl::Status st = DropFrom(b, fill.max + Point{0, 1}); !st.ok()) {
          return st;
        }
        if (b[fill.max + Point{0, 1}] != '~' &&
            b[fill.max + Point{0, 1}] != '#') {
          filling = false;
          break;
        }
      }
      if (b[fill.max + Point{1, 0}] == '|') {
        // Hit an existing drop.
        filling = false;
        break;
      }
      fill.max += Point{1, 0};
      if (!b.OnBoard(fill.max)) return Error("Off edge");
    }
    if (filling) {
      VLOG(1) << "Fill range: " << fill.min << "-" << fill.max;
      for (Point p : fill) b[p] = '~';
    } else {
      VLOG(1) << "Drop range: " << fill.min << "-" << fill.max;
      for (Point p : fill) b[p] = '|';
    }
    if (filling) {
      cur -= Point{0, 1};
      if (cur.y < start.y) {
        filling = false;
      }
    }
  }
  return absl::OkStatus();
}

absl::Status FillWithWater(CharBoard& b) {
  Point start;
  for (int x = 0; x < b.width(); ++x) {
    start = Point{x, 0};
    if (b[start] == '+') break;
  }
  if (!b.OnBoard(start)) return Error("Can't find spring");
  return DropFrom(b, start + Point{0, 1});
}

absl::StatusOr<CharBoard> Parse(absl::Span<absl::string_view> input,
                                int* min_y) {
  std::vector<PointRectangle> strips;
  PointRectangle grid = {{500, 0}, {500, 0}};
  *min_y = std::numeric_limits<int>::max();
  for (absl::string_view row : input) {
    int fixed;
    int r1;
    int r2;
    PointRectangle next;
    if (RE2::FullMatch(row, "x=(\\d+), y=(\\d+)\\.\\.(\\d+)", &fixed, &r1,
                       &r2)) {
      next = {{fixed, r1}, {fixed, r2}};
      *min_y = std::min(*min_y, r1);
      *min_y = std::min(*min_y, r2);
    } else if (RE2::FullMatch(row, "y=(\\d+), x=(\\d+)\\.\\.(\\d+)", &fixed,
                              &r1, &r2)) {
      next = {{r1, fixed}, {r2, fixed}};
      *min_y = std::min(*min_y, fixed);
    } else {
      return Error("Bad input: ", row);
    }
    grid.ExpandInclude(next.min);
    grid.ExpandInclude(next.max);
    strips.push_back(next);
  }
  --grid.min.x;
  ++grid.max.x;
  CharBoard b(grid.max.x - grid.min.x + 1, grid.max.y - grid.min.y + 1);
  for (Point p : b.range()) b[p] = '.';
  b[Point{500, 0} - grid.min] = '+';
  for (PointRectangle r : strips) {
    for (Point p : r) {
      b[p - grid.min] = '#';
    }
  }
  return b;
}
// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_17::Part1(
    absl::Span<absl::string_view> input) const {
  int min_y;
  absl::StatusOr<CharBoard> b = Parse(input, &min_y);
  if (!b.ok()) return b.status();
  VLOG_IF(1, b->height() < 100) << "Init:\n" << *b;
  if (VLOG_IS_ON(2) && b->height() >= 100) {
    VLOG(2) << "Init:" << *b;
  }
  if (absl::Status st = FillWithWater(*b); !st.ok()) {
    VLOG(2) << "Final:" << *b;
    return st;
  }
  VLOG_IF(1, b->height() < 100) << "Final:\n" << *b;
  if (VLOG_IS_ON(2) && b->height() >= 100) {
    VLOG(2) << "Final:" << *b;
  }
  int count = 0;
  for (Point p : b->range()) {
    if (p.y < min_y) continue;
    if ((*b)[p] == '~' || (*b)[p] == '|') ++count;
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2018_17::Part2(
    absl::Span<absl::string_view> input) const {
  int min_y;
  absl::StatusOr<CharBoard> b = Parse(input, &min_y);
  if (!b.ok()) return b.status();

  if (absl::Status st = FillWithWater(*b); !st.ok()) {
    return st;
  }
  int count = 0;
  for (Point p : b->range()) {
    if (p.y < min_y) continue;
    if ((*b)[p] == '~') ++count;
  }
  return IntReturn(count);
}

}  // namespace advent_of_code

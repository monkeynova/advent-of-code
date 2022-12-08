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

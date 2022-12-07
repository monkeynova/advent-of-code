// https://adventofcode.com/2019/day/3
//
// --- Day 3: Crossed Wires ---
// ----------------------------
//
// The gravity assist was successful, and you're well on your way to the
// Venus refuelling station. During the rush back on Earth, the fuel
// management system wasn't completely installed, so that's next on the
// priority list.
//
// Opening the front panel reveals a jumble of wires. Specifically, two
// wires are connected to a central port and extend outward on a grid.
// You trace the path each wire takes as it leaves the central port, one
// wire per line of text (your puzzle input).
//
// The wires twist and turn, but the two wires occasionally cross paths.
// To fix the circuit, you need to find the intersection point closest to
// the central port. Because the wires are on a grid, use the Manhattan
// distance for this measurement. While the wires do technically cross
// right at the central port where they both start, this point does not
// count, nor does a wire count as crossing with itself.
//
// For example, if the first wire's path is R8,U5,L5,D3, then starting
// from the central port (o), it goes right 8, up 5, left 5, and finally
// down 3:
//
// ...........
// ...........
// ...........
// ....+----+.
// ....|....|.
// ....|....|.
// ....|....|.
// .........|.
// .o-------+.
// ...........
//
// Then, if the second wire's path is U7,R6,D4,L4, it goes up 7, right 6,
// down 4, and left 4:
//
// ...........
// .+-----+...
// .|.....|...
// .|..+--X-+.
// .|..|..|.|.
// .|.-   X   --+.|.
// .|..|....|.
// .|.......|.
// .o-------+.
// ...........
//
// These wires cross at two locations (marked X), but the lower-left one
// is closer to the central port: its distance is 3 + 3 = 6.
//
// Here are a few more examples:
//
// * R75,D30,R83,U83,L12,D49,R71,U7,L72
// U62,R66,U55,R34,D71,R55,D58,R83 = distance 159
//
// * R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51
// U98,R91,D20,R16,D67,R40,U7,R15,U6,R7 = distance 135
//
// What is the Manhattan distance from the central port to the closest
// intersection?
//
// --- Part Two ---
// ----------------
//
// It turns out that this circuit is very timing-sensitive; you actually
// need to minimize the signal delay.
//
// To do this, calculate the number of steps each wire takes to reach
// each intersection; choose the intersection where the sum of both
// wires' steps is lowest. If a wire visits a position on the grid
// multiple times, use the steps value from the first time it visits that
// position when calculating the total value of a specific intersection.
//
// The number of steps a wire takes is the total number of grid squares
// the wire has entered to get to that location, including the
// intersection being considered. Again consider the example from above:
//
// ...........
// .+-----+...
// .|.....|...
// .|..+--X-+.
// .|..|..|.|.
// .|.-X--+.|.
// .|..|....|.
// .|.......|.
// .o-------+.
// ...........
//
// In the above example, the intersection closest to the central port is
// reached after 8+5+5+2 = 20 steps by the first wire and 7+6+4+3 = 20
// steps by the second wire for a total of 20+20 = 40 steps.
//
// However, the top-right intersection is better: the first wire takes
// only 8+5+2 = 15 and the second wire takes only 7+6+2 = 15, a total of
// 15+15 = 30 steps.
//
// Here are the best steps for the extra examples from above:
//
// * R75,D30,R83,U83,L12,D49,R71,U7,L72
// U62,R66,U55,R34,D71,R55,D58,R83 = 610 steps
//
// * R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51
// U98,R91,D20,R16,D67,R40,U7,R15,U6,R7 = 410 steps
//
// What is the fewest combined steps the wires must take to reach an
// intersection?

#include "advent_of_code/2019/day03/day03.h"

#include <limits>

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"

namespace advent_of_code {
namespace {

struct Line {
  Point from;
  Point to;
};

absl::StatusOr<std::vector<Line>> Parse(absl::string_view input) {
  std::vector<absl::string_view> runs = absl::StrSplit(input, ",");

  std::vector<Line> ret;
  ret.reserve(runs.size());
  Point cur = Cardinal::kOrigin;
  for (absl::string_view run : runs) {
    Line next;
    next.from = cur;
    next.to = cur;
    if (run.size() < 2) {
      return absl::InvalidArgumentError(
          absl::StrCat("Bad run (length): ", run));
    }
    int run_len;
    if (!absl::SimpleAtoi(run.substr(1), &run_len)) {
      return absl::InvalidArgumentError(absl::StrCat("Bad run (atoi): ", run));
    }
    switch (run[0]) {
      case 'R': {
        next.to.x += run_len;
        break;
      }
      case 'L': {
        next.to.x -= run_len;
        break;
      }
      case 'U': {
        next.to.y += run_len;
        break;
      }
      case 'D': {
        next.to.y -= run_len;
        break;
      }
      default: {
        return absl::InvalidArgumentError(absl::StrCat("Bad run (dir): ", run));
      }
    }

    cur = next.to;
    ret.push_back(next);
  }

  return ret;
}

bool Between(int test, int bound1, int bound2) {
  if (bound1 < bound2) {
    return bound1 <= test && test <= bound2;
  }
  return bound2 <= test && test <= bound1;
}

bool Overlap(int bound11, int bound12, int bound21, int bound22, int* out) {
  if (bound11 > bound12) std::swap(bound11, bound12);
  if (bound21 > bound22) std::swap(bound21, bound22);
  if (Between(bound11, bound21, bound22)) {
    *out = bound11;
    return true;
  }
  if (Between(bound21, bound11, bound12)) {
    *out = bound21;
    return true;
  }
  return false;
}

absl::StatusOr<absl::optional<Point>> Intersect(Line l1, Line l2) {
  if (l1.from.x == l1.to.x) {
    if (l2.from.x == l2.to.x) {
      if (l1.from.x == l2.from.x) {
        int miny;
        if (Overlap(l1.from.y, l1.to.y, l2.from.y, l2.to.y, &miny)) {
          return Point{l1.from.x, miny};
        }
        // No intersect.
      }
      // No intersect.
    } else if (l2.from.y == l2.to.y) {
      if (Between(l1.from.x, l2.from.x, l2.to.x) &&
          Between(l2.from.y, l1.from.y, l1.to.y)) {
        return Point{l1.from.x, l2.from.y};
      }
      // No intersect.
    } else {
      return absl::InvalidArgumentError("l2 isn't axis aligned");
    }
  } else if (l1.from.y == l1.to.y) {
    if (l2.from.x == l2.to.x) {
      if (Between(l2.from.x, l1.from.x, l1.to.x) &&
          Between(l1.from.y, l2.from.y, l2.to.y)) {
        return Point{l2.from.x, l1.from.y};
      }
      // No intersect.
    } else if (l2.from.y == l2.to.y) {
      if (l1.from.y == l2.from.y) {
        int minx;
        if (Overlap(l1.from.x, l1.to.x, l2.from.x, l2.to.x, &minx)) {
          return Point{minx, l1.from.y};
        }
        // No intersect.
      }
      // No intersect.
    } else {
      return absl::InvalidArgumentError("l2 isn't axis aligned");
    }
  } else {
    return absl::InvalidArgumentError("l1 isn't axis aligned");
  }
  return absl::nullopt;
}

absl::StatusOr<std::vector<Point>> Intersect(std::vector<Line> wire1,
                                             std::vector<Line> wire2) {
  std::vector<Point> ret;
  for (const auto& l1 : wire1) {
    for (const auto& l2 : wire2) {
      absl::StatusOr<absl::optional<Point>> intersect = Intersect(l1, l2);
      if (!intersect.ok()) return intersect.status();
      if (*intersect) {
        ret.push_back(**intersect);
      }
    }
  }

  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_03::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) {
    return absl::InvalidArgumentError("input does not contain 2 lines");
  }
  absl::StatusOr<std::vector<Line>> wire1 = Parse(input[0]);
  if (!wire1.ok()) return wire1.status();
  absl::StatusOr<std::vector<Line>> wire2 = Parse(input[1]);
  if (!wire2.ok()) return wire2.status();

  absl::StatusOr<std::vector<Point>> overlap = Intersect(*wire1, *wire2);
  if (!overlap.ok()) return overlap.status();
  absl::optional<Point> min;
  for (const auto& point : *overlap) {
    if (point.dist() > 0 && (!min || min->dist() > point.dist())) {
      min = point;
    }
  }

  if (!min) {
    return absl::InvalidArgumentError("No intersect found");
  }

  return IntReturn(min->dist());
}

absl::StatusOr<bool> PointOnLine(Point point, Line line) {
  if (line.from.x == line.to.x) {
    return point.x == line.from.x && Between(point.y, line.from.y, line.to.y);
  } else if (line.from.y == line.to.y) {
    return point.y == line.from.y && Between(point.x, line.from.x, line.to.x);
  }
  return absl::InvalidArgumentError("line isn't axis aligned");
}

absl::StatusOr<int> CostToOverlap(Point intersect,
                                  const std::vector<Line>& wire) {
  std::vector<int> cost_to_start(wire.size() + 1, 0);
  for (int i = 0; i < wire.size(); ++i) {
    absl::StatusOr<bool> on_line = PointOnLine(intersect, wire[i]);
    if (!on_line.ok()) return on_line.status();

    if (*on_line) {
      return cost_to_start[i] + (wire[i].from - intersect).dist();
    }
    cost_to_start[i + 1] =
        cost_to_start[i] + (wire[i].from - wire[i].to).dist();
  }

  return absl::InvalidArgumentError("Point not found on wire");
}

absl::StatusOr<std::string> Day_2019_03::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) {
    return absl::InvalidArgumentError("input does not contain 2 lines");
  }
  absl::StatusOr<std::vector<Line>> wire1 = Parse(input[0]);
  if (!wire1.ok()) return wire1.status();
  absl::StatusOr<std::vector<Line>> wire2 = Parse(input[1]);
  if (!wire2.ok()) return wire2.status();

  absl::StatusOr<std::vector<Point>> overlap = Intersect(*wire1, *wire2);
  if (!overlap.ok()) return overlap.status();
  absl::optional<Point> min;
  int min_score = std::numeric_limits<int>::max();
  for (const auto& point : *overlap) {
    if (point.dist() > 0) {
      absl::StatusOr<int> wire1_score = CostToOverlap(point, *wire1);
      if (!wire1_score.ok()) return wire1_score.status();
      absl::StatusOr<int> wire2_score = CostToOverlap(point, *wire2);
      if (!wire2_score.ok()) return wire2_score.status();

      int score = *wire1_score + *wire2_score;
      if (score < min_score) {
        min = point;
        min_score = score;
      }
    }
  }

  if (!min) {
    return absl::InvalidArgumentError("No intersect found");
  }

  return IntReturn(min_score);
}

}  // namespace advent_of_code

#include "advent_of_code/2018/day11/day11.h"

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

int PowerLevel(int serial, Point p) {
  int rack = p.x + 10;
  int power = p.y * rack;
  power += serial;
  power *= rack;
  power = (power / 100) % 10;
  power -= 5;
  VLOG(2) << "Power(" << p << ") = " << power;
  return power;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day11_2018::Part1(
    absl::Span<absl::string_view> input) const {
  if (PowerLevel(57, Point{122,79}) != -5) return Error("Integrity check 1");
  if (PowerLevel(39, Point{217,196}) != 0) return Error("Integrity check 2");
  if (PowerLevel(71, Point{101,153}) != 4) return Error("Integrity check 3");
  if (input.size() != 1) return Error("Bad size");
  int serial;
  if (!absl::SimpleAtoi(input[0], &serial)) return Error("Not an int");
  absl::flat_hash_map<Point, int> power;
  PointRectangle r{{1, 1}, {300, 300}};
  for (Point p : r) power[p] = PowerLevel(serial, p);
  PointRectangle search{{1, 1}, {298, 298}};
  int max_power = std::numeric_limits<int>::min();
  Point max_power_point;
  for (Point p : search) {
    int power_sum = 0;
    for (Point p2 : PointRectangle{p, p + Point{2, 2}}) {
      power_sum += power[p2];
    }
    VLOG(1) << "PowerSum(" << p << ") = " << power_sum;
    if (power_sum > max_power) {
      max_power = power_sum;
      max_power_point = p;
    }
  }

  return std::vector<std::string>{max_power_point.DebugString()};
}

absl::StatusOr<std::vector<std::string>> Day11_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

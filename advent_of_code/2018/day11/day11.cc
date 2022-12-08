#include "advent_of_code/2018/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
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

absl::StatusOr<std::string> Day_2018_11::Part1(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    if (PowerLevel(57, Point{122, 79}) != -5) return Error("Integrity check 1");
    if (PowerLevel(39, Point{217, 196}) != 0) return Error("Integrity check 2");
    if (PowerLevel(71, Point{101, 153}) != 4) return Error("Integrity check 3");
  }
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

  return PointReturn(max_power_point);
}

absl::StatusOr<std::string> Day_2018_11::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  int serial;
  if (!absl::SimpleAtoi(input[0], &serial)) return Error("Not an int");
  absl::flat_hash_map<Point, int> power;
  PointRectangle r{{1, 1}, {300, 300}};
  for (Point p : r) power[p] = PowerLevel(serial, p);
  int max_power = std::numeric_limits<int>::min();
  Point3 max_power_point;
  absl::flat_hash_map<Point3, int> sums;
  for (int s = 0; s < 300; s++) {
    // absl::flat_hash_map<Point, int> this_sums;
    PointRectangle search = r;
    search.max -= Point{s, s};
    for (Point p : search) {
      int power_sum = 0;
      if (s == 0) {
        power_sum = power[p];
      } else if ((s + 1) % 2 == 0) {
        int half_s = (s + 1) / 2;
        power_sum += sums[{p.x, p.y, half_s}];
        power_sum += sums[{p.x + half_s, p.y, half_s}];
        power_sum += sums[{p.x, p.y + half_s, half_s}];
        power_sum += sums[{p.x + half_s, p.y + half_s, half_s}];
      } else {
        // Round down.
        int half_s = (s + 1) / 2;
        power_sum += sums[{p.x, p.y, half_s + 1}];
        power_sum += sums[{p.x + half_s + 1, p.y, half_s}];
        power_sum += sums[{p.x, p.y + half_s + 1, half_s}];
        power_sum += sums[{p.x + half_s, p.y + half_s, half_s + 1}];
        // Was included in upper left and lower right.
        power_sum -= power[p + Point{half_s, half_s}];
      }
      sums[{p.x, p.y, s + 1}] = power_sum;
      if (power_sum > max_power) {
        max_power = power_sum;
        max_power_point.x = p.x;
        max_power_point.y = p.y;
        max_power_point.z = s + 1;
      }
    }
    // prev_sums = std::move(this_sums);
  }

  return PointReturn(max_power_point);
}

}  // namespace advent_of_code

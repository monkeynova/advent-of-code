// https://adventofcode.com/2018/day/11
//
// --- Day 11: Chronal Charge ---
// ------------------------------
// 
// You watch the Elves and their sleigh fade into the distance as they
// head toward the North Pole.
// 
// Actually, you're the one fading. The falling sensation returns.
// 
// The low fuel warning light is illuminated on your wrist-mounted
// device. Tapping it once causes it to project a hologram of the
// situation: a 300x300 grid of fuel cells and their current power
// levels, some negative. You're not sure what negative power means in
// the context of time travel, but it can't be good.
// 
// Each fuel cell has a coordinate ranging from 1 to 300 in both the X
// (horizontal) and Y (vertical) direction. In X,Y notation, the top-left
// cell is 1,1, and the top-right cell is 300,1.
// 
// The interface lets you select any 3x3 square of fuel cells. To
// increase your chances of getting to your destination, you decide to
// choose the 3x3 square with the largest total power.
// 
// The power level in a given fuel cell can be found through the
// following process:
// 
// * Find the fuel cell's rack ID, which is its X coordinate plus 10.
// 
// * Begin with a power level of the rack ID times the Y coordinate.
// 
// * Increase the power level by the value of the grid serial number
// (your puzzle input).
// 
// * Set the power level to itself multiplied by the rack ID.
// 
// * Keep only the hundreds digit of the power level (so 12345 becomes
// 3; numbers with no hundreds digit become 0).
// 
// * Subtract 5 from the power level.
// 
// For example, to find the power level of the fuel cell at 3,5 in a grid
// with serial number 8:
// 
// * The rack ID is 3 + 10 = 13.
// 
// * The power level starts at 13 * 5 = 65.
// 
// * Adding the serial number produces 65 + 8 = 73.
// 
// * Multiplying by the rack ID produces 73 * 13 = 949.
// 
// * The hundreds digit of 949 is 9.
// 
// * Subtracting 5 produces 9 - 5 = 4.
// 
// So, the power level of this fuel cell is 4.
// 
// Here are some more example power levels:
// 
// * Fuel cell at 122,79, grid serial number 57: power level -5.
// 
// * Fuel cell at 217,196, grid serial number 39: power level 0.
// 
// * Fuel cell at 101,153, grid serial number 71: power level 4.
// 
// Your goal is to find the 3x3 square which has the largest total power.
// The square must be entirely within the 300x300 grid. Identify this
// square using the X,Y coordinate of its top-left fuel cell. For
// example:
// 
// For grid serial number 18, the largest total 3x3 square has a top-left
// corner of 33,45 (with a total power of 29); these fuel cells appear in
// the middle of this 5x5 region:
// 
// -2  -4   4   4   4
// -4      4   4   4     -5
// 4      3   3   4     -4
// 1      1   2   4     -3
// -1   0   2  -5  -2
// 
// For grid serial number 42, the largest 3x3 square's top-left is 21,61
// (with a total power of 30); they are in the middle of this region:
// 
// -3   4   2   2   2
// -4      4   3   3      4
// -5      3   3   4     -4
// 4      3   3   4     -3
// 3   3   3  -5  -1
// 
// What is the X,Y coordinate of the top-left fuel cell of the 3x3 square
// with the largest total power?
//
// --- Part Two ---
// ----------------
// 
// You discover a dial on the side of the device; it seems to let you
// select a square of any size, not just 3x3. Sizes from 1x1 to 300x300
// are supported.
// 
// Realizing this, you now must find the square of any size with the
// largest total power. Identify this square by including its size as a
// third parameter after the top-left coordinate: a 9x9 square with a
// top-left corner of 3,5 is identified as 3,5,9.
// 
// For example:
// 
// * For grid serial number 18, the largest total square (with a total
// power of 113) is 16x16 and has a top-left corner of 90,269, so its
// identifier is 90,269,16.
// 
// * For grid serial number 42, the largest total square (with a total
// power of 119) is 12x12 and has a top-left corner of 232,251, so
// its identifier is 232,251,12.
// 
// What is the X,Y,size identifier of the square with the largest total
// power?


#include "advent_of_code/2018/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
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

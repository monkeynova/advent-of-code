// https://adventofcode.com/2017/day/3
//
// --- Day 3: Spiral Memory ---
// ----------------------------
//
// You come across an experimental new kind of memory stored on an
// infinite two-dimensional grid.
//
// Each square on the grid is allocated in a spiral pattern starting at a
// location marked 1 and then counting up while spiraling outward. For
// example, the first few squares are allocated like this:
//
// 17  16  15  14  13
// 18   5   4   3  12
// 19   6   1   2  11
// 20   7   8   9  10
// 21  22  23---> ...
//
// While this is very space-efficient (no squares are skipped), requested
// data must be carried back to square 1 (the location of the only access
// port for this memory system) by programs that can only move up, down,
// left, or right. They always take the shortest path: the Manhattan
// Distance between the location of the data and square 1.
//
// For example:
//
// * Data from square 1 is carried 0 steps, since it's at the access
// port.
//
// * Data from square 12 is carried 3 steps, such as: down, left, left.
//
// * Data from square 23 is carried only 2 steps: up twice.
//
// * Data from square 1024 must be carried 31 steps.
//
// How many steps are required to carry the data from the square
// identified in your puzzle input all the way to the access port?
//
// --- Part Two ---
// ----------------
//
// As a stress test on the system, the programs here clear the grid and
// then store the value 1 in square 1. Then, in the same allocation order
// as shown above, they store the sum of the values in all adjacent
// squares, including diagonals.
//
// So, the first few squares' values are chosen as follows:
//
// * Square 1 starts with the value 1.
//
// * Square 2 has only one adjacent filled square (with value 1), so it
// also stores 1.
//
// * Square 3 has both of the above squares as neighbors and stores the
// sum of their values, 2.
//
// * Square 4 has all three of the aforementioned squares as neighbors
// and stores the sum of their values, 4.
//
// * Square 5 only has the first and fourth squares as neighbors, so it
// gets the value 5.
//
// Once a square is written, its value does not change. Therefore, the
// first few squares would receive the following values:
//
// 147  142  133  122   59
// 304    5    4    2   57
// 330   10    1    1   54
// 351   11   23   25   26
// 362  747  806--->   ...
//
// What is the first value written that is larger than your puzzle input?

#include "advent_of_code/2017/day03/day03.h"

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

Point FromPosition(int position) {
  int box_size = 1;
  Point ret = {0, 0};
  if (position == 0) return ret;
  while (position > box_size * box_size) {
    box_size += 2;
    ret += box_size == 1 ? Point{1, 0} : Point{1, 1};
  }
  position -= (box_size - 2) * (box_size - 2);
  if (position > 0) {
    int up = std::min(position, box_size - 1);
    ret += up * Cardinal::kNorth;
    position -= up;
  }
  if (position > 0) {
    int left = std::min(position, box_size - 1);
    ret += left * Cardinal::kWest;
    position -= left;
  }
  if (position > 0) {
    int down = std::min(position, box_size - 1);
    ret += down * Cardinal::kSouth;
    position -= down;
  }
  if (position > 0) {
    int right = std::min(position, box_size - 1);
    ret += right * Cardinal::kEast;
    position -= right;
  }
  CHECK(position == 0);
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_03::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> nums = ParseAsInts(input);
  if (!nums.ok()) return nums.status();
  if (nums->size() != 1) return Error("Bad size");
  return IntReturn(FromPosition(nums->at(0)).dist());
}

absl::StatusOr<std::string> Day_2017_03::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> nums = ParseAsInts(input);
  if (!nums.ok()) return nums.status();
  if (nums->size() != 1) return Error("Bad size");
  int min_val = nums->at(0);
  absl::flat_hash_map<Point, int> sums;
  sums[{0, 0}] = 1;
  for (int i = 2;; ++i) {
    Point p = FromPosition(i);
    int this_sum = 0;
    for (Point dir : Cardinal::kEightDirs) {
      auto it = sums.find(p + dir);
      if (it != sums.end()) this_sum += it->second;
    }
    if (this_sum > min_val) return IntReturn(this_sum);
    LOG(INFO) << p << ": " << this_sum;
    if (this_sum == 0) return Error("Zero sum!");
    sums[p] = this_sum;
  }
  return Error("Left infinite loop");
}

}  // namespace advent_of_code

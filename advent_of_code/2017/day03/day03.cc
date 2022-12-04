// http://adventofcode.com/2017/day/03

#include "advent_of_code/2017/day03/day03.h"

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

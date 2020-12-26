#include "advent_of_code/2017/day03/day03.h"

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

absl::StatusOr<std::vector<std::string>> Day03_2017::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> nums = ParseAsInts(input);
  if (!nums.ok()) return nums.status();
  if (nums->size() != 1) return Error("Bad size");
  return IntReturn(FromPosition(nums->at(0)).dist());
}

absl::StatusOr<std::vector<std::string>> Day03_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code

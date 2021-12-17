#include "advent_of_code/2021/day17/day17.h"

#include "absl/algorithm/container.h"
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

absl::optional<int> Fire(Point v0, PointRectangle target) {
  Point cur = {0, 0};
  Point v = v0;
  int max_y = cur.y;
  while (true) {
    // VLOG(1) << "cur=" << cur << "; v=" << v;
    cur += v;
    max_y = std::max(max_y, cur.y);
    if (v.x > 0) --v.x;
    if (v.x < 0) ++v.x;
    --v.y;
    if (cur.x >= target.min.x && cur.x <= target.max.x &&
        cur.y >= target.min.y && cur.y <= target.max.y) {
      return max_y;
    }
    if (cur.x > target.max.x) return {};
    if (cur.y < target.min.y) return {};
  }

  LOG(FATAL) << "left infinite loop";
}
// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_17::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int minx, maxx, miny, maxy;
  if (!RE2::FullMatch(input[0], "target area: x=(-?\\d+)..(-?\\d+), y=(-\\d+)..(-?\\d+)",
                      &minx, &maxx, &miny, &maxy)) {
      return Error("Bad line");
  }
  PointRectangle target{{minx, miny}, {maxx, maxy}};
  int64_t max_max = 0;
  for (int x = 1; x < 2000; x++) {
    for (int y = 0; y < 2000; y++) {
      Point v0{x, y};
      absl::optional<int64_t> max_y = Fire(v0, target);
      if (!max_y) continue;
      // VLOG(1) << v0 << " => " << *max_y;
      max_max = std::max(*max_y, max_max);
    }
  }
  return IntReturn(max_max);
}

absl::StatusOr<std::string> Day_2021_17::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int minx, maxx, miny, maxy;
  if (!RE2::FullMatch(input[0], "target area: x=(-?\\d+)..(-?\\d+), y=(-\\d+)..(-?\\d+)",
                      &minx, &maxx, &miny, &maxy)) {
      return Error("Bad line");
  }
  PointRectangle target{{minx, miny}, {maxx, maxy}};
  int64_t count = 0;
  for (int x = 1; x < 4000; x++) {
    for (int y = -4000; y < 4000; y++) {
      Point v0{x, y};
      absl::optional<int64_t> max_y = Fire(v0, target);
      if (!max_y) continue;
      // VLOG(1) << v0 << " => " << *max_y;
      ++count;
    }
  }
  return IntReturn(count);
}

}  // namespace advent_of_code

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

absl::StatusOr<PointRectangle> VelocityBounds(PointRectangle target) {
  // TODO(@monkeynova): We don't need to iterate over the full cartesian
  // product. x and y update independently, so we should be able to solve
  // each independently.
  if (target.min.x <= 0) {
    return absl::UnimplementedError("target must have positive x");
  }
  if (target.min.y > 0) {
    return absl::UnimplementedError("target must have negative y");
  }
  // the y coordinate will always go back through 0, with oppositve velocity.
  // This means that if v0.y > -miny it will skip the target.
  int min_vx = 1;
  while (min_vx * (min_vx + 1) / 2 < target.min.x) ++min_vx;
  return PointRectangle{{min_vx, target.min.y}, {target.max.x, -target.min.y}};
}

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
    if (target.Contains(cur)) return max_y;
    if (cur.x > target.max.x) return {};
    if (cur.y < target.min.y) return {};
  }

  LOG(FATAL) << "left infinite loop";
}

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
  absl::StatusOr<PointRectangle> vbound = VelocityBounds(target);
  if (!vbound.ok()) return vbound.status();
  for (Point v0 : *vbound) {
    absl::optional<int64_t> max_y = Fire(v0, target);
    if (!max_y) continue;
    // VLOG(1) << v0 << " => " << *max_y;
    max_max = std::max(*max_y, max_max);
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
  absl::StatusOr<PointRectangle> vbound = VelocityBounds(target);
  if (!vbound.ok()) return vbound.status();
  for (Point v0 : *vbound) {
    absl::optional<int64_t> max_y = Fire(v0, target);
    if (!max_y) continue;
    // VLOG(1) << v0 << " => " << *max_y;
    ++count;
  }
  return IntReturn(count);
}

}  // namespace advent_of_code

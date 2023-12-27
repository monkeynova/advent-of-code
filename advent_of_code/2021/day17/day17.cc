#include "advent_of_code/2021/day17/day17.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::StatusOr<PointRectangle> VelocityBounds(PointRectangle target) {
  if (target.min.x <= 0) {
    return absl::UnimplementedError("target must have positive x");
  }
  if (target.min.y > 0) {
    return absl::UnimplementedError("target must have negative y");
  }
  // In order to reach the bound, with a drag decay, we can start at the first
  // step squad >= min_x.
  int min_vx = 1;
  while (min_vx * (min_vx + 1) / 2 < target.min.x) ++min_vx;
  // the y coordinate will always go back through 0, with oppositve velocity.
  // This means that if v0.y >= -miny it will skip the target.
  return PointRectangle{{min_vx, target.min.y}, {target.max.x, -target.min.y - 1}};
}

std::optional<int> MaxYFromFire(Point v0, PointRectangle target) {
  Point cur = {0, 0};
  Point v = v0;
  Point a = {-1, -1};
  if (v.x < 0) a.x = 1;

  int max_y = cur.y;
  for (int step = 0; true; ++step) {
    VLOG(2) << "cur=" << cur << "; v=" << v;
    cur += v;
    max_y = std::max(max_y, cur.y);
    v += a;
    if (v.x == 0) a.x = 0;
    if (target.Contains(cur)) {
      VLOG(1) << "In target @" << step;
      return max_y;
    }
    if (cur.x > target.max.x) return {};
    if (cur.y < target.min.y) return {};
  }

  LOG(FATAL) << "left infinite loop";
}

Interval1D SolveQuadraticInteger(int a, int b, int c) {
  int d = b * b - 4 * a * c;
  if (d < 0) return Interval1D();
  int min = ceil((-b - sqrt(d)) / (2 * a));
  int max = floor((-b + sqrt(d)) / (2 * a));
  return Interval1D(min, max + 1);
}

Interval1D StepsInRange(int v0, int x_min, int x_max, bool stop_at_top) {
  // x = SUM(v0 - i, i=0..s)
  // x = v0 * (s + 1) - s * (s + 1) / 2
  // v0 * s + v0 - s * (s + 1) / 2 >= x_min
  // 0 >= s * s - (2 * v0 - 1) * s + 2 * x_min - 2 * v0
  Interval1D min = SolveQuadraticInteger(1, -2 * v0 + 1, 2 * x_min - 2 * v0);
  if (min.empty()) return min;

  Interval1D stop;
  if (stop_at_top) {
    stop = Interval1D::OpenTop(ceil(-(-2 * v0 + 1) / (2.0 * 1)));
  }
  min = min.Union(stop);
  Interval1D max = SolveQuadraticInteger(1, -2 * v0 + 1, 2 * (x_max + 1) - 2 * v0);
  if (!max.empty()) {
    max = max.Union(stop);
  }
  return min.Minus(max).Intersect(Interval1D::OpenTop(0));
}

class IntervalSet {
 public:
  IntervalSet() = default;

  void Add(int x, Interval1D x_set) {
    if (x_set.empty()) return;
    set_[x_set].insert(x);
  }

  int CountIntersect(Interval1D test) const {
    if (test.empty()) return 0;
    absl::flat_hash_set<int> ret;
    for (const auto& [i, s] : set_) {
      if (test.HasOverlap(i)) {
        if (ret.empty()) {
          ret = s;
        } else {
          for (int i : s) {
            ret.insert(i);
          }
        }
      }
    }
    return ret.size();
  }

 private:
  absl::flat_hash_map<Interval1D, absl::flat_hash_set<int>> set_;
};

}  // namespace

absl::StatusOr<std::string> Day_2021_17::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int minx, maxx, miny, maxy;
  if (!RE2::FullMatch(input[0], "target area: x=(-?\\d+)\\.\\.(-?\\d+), y=(-\\d+)\\.\\.(-?\\d+)",
                      &minx, &maxx, &miny, &maxy)) {
    return Error("Bad line");
  }
  if (miny >= 0) return Error("Postive miny not supported");
  PointRectangle target{{minx, miny}, {maxx, maxy}};
  ASSIGN_OR_RETURN(PointRectangle vbound, VelocityBounds(target));
  if (StepsInRange(vbound.max.y, miny, maxy, /*stop_at_top=*/false).Intersect(
        StepsInRange(vbound.min.x, minx, maxx, /*stop_at_top=*/true)).empty()) {
    return Error("maxy 'optimization' doesn't land in range");
  }
  int maxy_from_vy = vbound.max.y * (vbound.max.y + 1) / 2;
  if (run_audit()) {
    std::optional<int> maxy = MaxYFromFire(Point{vbound.min.x, vbound.max.y}, target);
    if (!maxy) return Error("Audit: MaxYFromFire doesn't hit");
    if (*maxy != maxy_from_vy) {
      return Error("Audit: MaxYFromFire doesn't hit");
    }
  }
  return AdventReturn(maxy_from_vy);
}

absl::StatusOr<std::string> Day_2021_17::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int minx, maxx, miny, maxy;
  if (!RE2::FullMatch(input[0], "target area: x=(-?\\d+)\\.\\.(-?\\d+), y=(-\\d+)\\.\\.(-?\\d+)",
                      &minx, &maxx, &miny, &maxy)) {
    return Error("Bad line");
  }
  PointRectangle target{{minx, miny}, {maxx, maxy}};
  ASSIGN_OR_RETURN(PointRectangle vbound, VelocityBounds(target));
  IntervalSet x_set;
  for (int vx = vbound.min.x; vx <= vbound.max.x; ++vx) {
    x_set.Add(vx, StepsInRange(vx, minx, maxx, /*stop_at_top=*/true));
  }
  int64_t count = 0;
  for (int vy = vbound.min.y; vy <= vbound.max.y; ++vy) {
    count += x_set.CountIntersect(StepsInRange(vy, miny, maxy, /*stop_at_top=*/false));
  }
  return AdventReturn(count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2021, /*day=*/17, []() {
  return std::unique_ptr<AdventDay>(new Day_2021_17());
});

}  // namespace advent_of_code
